// Fill out your copyright notice in the Description page of Project Settings.


#include "LD8CharacterBase.h"

#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"

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
}

void ALD8CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/* ==================== Movement ==================== */

void ALD8CharacterBase::DoMove(float Right, float Forward)
{
	if (!GetController())
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
	Jump();
}

void ALD8CharacterBase::DoJumpEnd()
{
	StopJumping();
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

bool ALD8CharacterBase::Shoot()
{
	if (Gun == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Gun is NULL"), *GetActorLabel());
		return false;
	}

	return Gun->PullTrigger();
}


/* ==================== Health ==================== */

bool ALD8CharacterBase::IsDead() const
{
	return CurrentHP <= 0;
}

float ALD8CharacterBase::GetHealthPercent() const
{
	return CurrentHP / MaxHP;
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
