// Fill out your copyright notice in the Description page of Project Settings.


#include "LD8CharacterBase.h"

#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

#include "Weapon/LD8Gun.h"
#include "GameMode/LD8GameModeBase.h"


/* ==================== CharacterBase Lifecycle ==================== */

ALD8CharacterBase::ALD8CharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ALD8CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[%s] Generate"), *GetActorLabel());

	SpawnGun();

	// 기본 캡슐 절반 높이
	DefaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	// 기본 회전 설정 저장
	DefaultUseControllerRotationYaw = bUseControllerRotationYaw;
	DefaultOrientRotationToMovement = GetCharacterMovement()->bOrientRotationToMovement;
	DefaultUseControllerDesiredRotation = GetCharacterMovement()->bUseControllerDesiredRotation;

	// 메시 상대 위치 저장
	if (GetMesh() != nullptr)
	{
		DefaultMeshRelativeLocation = GetMesh()->GetRelativeLocation();
	}
}

void ALD8CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ALD8CharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(CurrentHP, DamageToApply); // CurrentHP보다 데미지가 클 경우 CurrentHP만큼만 데미지 적용
	CurrentHP -= DamageToApply;

	UE_LOG(LogTemp, Warning, TEXT("[%s] Health left: %f"), *GetActorLabel(), CurrentHP);

	if (IsDead())
	{
		ALD8GameModeBase* GameMode = GetWorld()->GetAuthGameMode<ALD8GameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy(); // 컨트롤러 연결 해제
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 콜리전 비활성화
	}

	return DamageToApply;
}


/* ==================== Movement ==================== */

void ALD8CharacterBase::DoMove(float Right, float Forward)
{
	if (!GetController())
		return;

	// 이동 입력이 없으면 LastMoveInput 초기화 후 종료
	const FVector2D MoveInput = FVector2D(Right, Forward);
	if (MoveInput.IsNearlyZero())
	{
		LastMoveInput = FVector2D::ZeroVector;
		return;
	}

	LastMoveInput = FVector2D(Right, Forward);

	if (bIsRolling)
		return;

	AddMovementInput(GetActorRightVector(), Right * MoveSpeedScale);
	AddMovementInput(GetActorForwardVector(), Forward * MoveSpeedScale);
}

void ALD8CharacterBase::DoLook(float Yaw, float Pitch)
{
	if (!GetController())
		return;

	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void ALD8CharacterBase::DoJumpStart()
{
	if (bIsRolling)
		return;

	Jump();
}

void ALD8CharacterBase::DoJumpEnd()
{
	StopJumping();
}

void ALD8CharacterBase::ClearMoveInput()
{
	LastMoveInput = FVector2D::ZeroVector;
}

void ALD8CharacterBase::DoRoll()
{
	if (!bCanRoll || bIsRolling)
		return;

	if (RollMontage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] RollMontage is NULL"), *GetActorLabel());
		return;
	}

	const FVector RollDirection = GetRollDirection();
	if (RollDirection.IsNearlyZero())
		return;

	bIsRolling = true;
	bCanRoll = false;

	// 구르기 중에는 컨트롤러 회전이 캐릭터 회전을 덮어쓰지 못하게
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	// 몽타주 재생 전에 구를 방향으로 캐릭터 회전
	const FRotator RollRotation = FRotator(0.0f, RollDirection.Rotation().Yaw, 0.0f);
	SetActorRotation(RollRotation);

	// 기존 이동 속도 제거
	//GetCharacterMovement()->StopMovementImmediately();

	// 히트박스 축소
	CurrentRollCapsuleHeightDelta = FMath::Max(0.0f, DefaultCapsuleHalfHeight - RollCapsuleHalfHeight);
	GetCapsuleComponent()->SetCapsuleHalfHeight(RollCapsuleHalfHeight, true);

	// 캡슐이 줄어든 만큼 Mesh를 위로 올려서 바닥에 묻히는 것 방지
	if (GetMesh() != nullptr)
	{
		GetMesh()->SetRelativeLocation(DefaultMeshRelativeLocation + FVector(0.0f, 0.0f, CurrentRollCapsuleHeightDelta));
	}

	// PlayerCharacter 쪽에서 카메라 보정할 수 있게 호출
	OnRollStarted(CurrentRollCapsuleHeightDelta);

	// 루트모션 이동 거리 배율 적용
	SetAnimRootMotionTranslationScale(RollRootMotionDistanceScale);

	// 루트모션 몽타주 재생
	const float MontageLength = PlayAnimMontage(RollMontage);

	if (MontageLength <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] RollMontage Play Failed"), *GetActorLabel());
		EndRoll();
	}
}

FVector ALD8CharacterBase::GetRollDirection() const
{
	FRotator BaseRotation = GetActorRotation();
	if (GetController() != nullptr)
	{
		BaseRotation = GetController()->GetControlRotation();
	}

	BaseRotation.Pitch = 0.0f;
	BaseRotation.Roll = 0.0f;

	const FVector ForwardDirection = FRotationMatrix(BaseRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(BaseRotation).GetUnitAxis(EAxis::Y);

	FVector RollDirection = FVector::ZeroVector;

	if (!LastMoveInput.IsNearlyZero())	// 이동 입력이 있으면 입력 방향으로 구르기
	{
		RollDirection = RightDirection * LastMoveInput.X + ForwardDirection * LastMoveInput.Y;
	}
	else								// 이동 입력이 없으면 캐릭터가 바라보는 방향으로 구르기
	{
		RollDirection = ForwardDirection;
	}

	RollDirection.Z = 0.0f;
	return RollDirection.GetSafeNormal();
}

void ALD8CharacterBase::EndRoll()
{
	bIsRolling = false;

	// 루트모션 이동 거리 배율 복구
	SetAnimRootMotionTranslationScale(1.0f);

	// 루트모션 이후 남은 속도 제거
	//GetCharacterMovement()->StopMovementImmediately();

	// 캡슐 복원 전에 현재 보정값 저장
	const float RollCapsuleHeightDelta = CurrentRollCapsuleHeightDelta;

	// 줄어든 캡슐 높이만큼 Actor를 먼저 위로 올림
	if (RollCapsuleHeightDelta > 0.0f)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, RollCapsuleHeightDelta), false);
	}

	// 히트박스 복원
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight, true);

	// Mesh 위치 복원
	if (GetMesh() != nullptr)
	{
		GetMesh()->SetRelativeLocation(DefaultMeshRelativeLocation);
	}

	CurrentRollCapsuleHeightDelta = 0.0f;

	// 카메라 위치 복원
	OnRollEnded();

	// 회전 설정 복구
	bUseControllerRotationYaw = DefaultUseControllerRotationYaw;
	GetCharacterMovement()->bOrientRotationToMovement = DefaultOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = DefaultUseControllerDesiredRotation;

	// 구르기 쿨타임 타이머 시작
	GetWorldTimerManager().ClearTimer(RollCooldownTimerHandle);
	GetWorldTimerManager().SetTimer(
		RollCooldownTimerHandle,
		this,
		&ALD8CharacterBase::ResetRollCooldown,
		RollCooldown,
		false
	);
}

void ALD8CharacterBase::ResetRollCooldown()
{
	bCanRoll = true;
}

void ALD8CharacterBase::OnRollStarted(float CapsuleHeightDelta)
{

}

void ALD8CharacterBase::OnRollEnded()
{

}

void ALD8CharacterBase::OnRollEndNotify()
{
	if (!bIsRolling)
		return;

	// 루트모션 몽타주가 끝나면 구르기 종료
	if (RollMontage != nullptr)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.1f, RollMontage);
		}
	}

	// 구르기 종료 처리
	EndRoll();
}


/* ==================== Attack ==================== */

void ALD8CharacterBase::SpawnGun()
{
	if (GunClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] GunClass is NULL"), *GetActorLabel());
		return;
	}

	Gun = GetWorld()->SpawnActor<ALD8Gun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponSocket);
	Gun->SetOwner(this);
}

bool ALD8CharacterBase::Shoot(float SpreadAngle)
{
	if (Gun == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Gun is NULL"), *GetActorLabel());
		return false;
	}

	return Gun->PullTrigger(SpreadAngle);
}


/* ==================== Getter ==================== */

bool ALD8CharacterBase::IsDead() const
{
	return CurrentHP <= 0;
}

float ALD8CharacterBase::GetHealthPercent() const
{
	return CurrentHP / MaxHP;
}

bool ALD8CharacterBase::IsRolling() const
{
	return bIsRolling;
}
