// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LD8PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Player/LD8SkillComponent.h"


/* ==================== CharacterBase Lifecycle ==================== */

// Sets default values
ALD8PlayerCharacter::ALD8PlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터가 컨트롤러 회전에 따라 회전
	bUseControllerRotationYaw = true;

	// 카메라 붐 생성 (캐릭터 뒤에 위치시키기 위함)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));	// CameraBoom 자체는 캐릭터 중심 위에 둠
	CameraBoom->SocketOffset = FVector(0.0f, 75.0f, 0.0f);			// 어깨 오프셋은 SocketOffset으로 처리
	CameraBoom->bUsePawnControlRotation = true;						// 카메라 붐이 컨트롤러 회전에 따라 회전

	// 팔로우 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;					// 카메라가 컨트롤러 회전에 따라 회전하지 않음

	// 스킬 컴포넌트 생성
	SkillComponent = CreateDefaultSubobject<ULD8SkillComponent>(TEXT("SkillComponent"));

	// 초기 상태 설정
	MaxHP = 100.0f;
	CurrentHP = MaxHP;
	MoveSpeedScale = 1.5f;
	JumpPowerScale = 2.0f;

	// CharacterMovement 세팅
	GetCharacterMovement()->BrakingDecelerationFalling = 50.0f;				// 공중 감속력
	GetCharacterMovement()->AirControl = 0.7f;								// 공중 제어
	GetCharacterMovement()->GravityScale = 2.0f;							// 중력 배율

	GetCharacterMovement()->GroundFriction = 8.0f;							// 마찰력
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;			// 감속력

	GetCharacterMovement()->MaxWalkSpeed *= MoveSpeedScale;					// 이동 속도
	GetCharacterMovement()->JumpZVelocity *= JumpPowerScale;				// 점프 힘
}

// Called when the game starts or when spawned
void ALD8PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터 기본 상태 저장
	if (CameraBoom != nullptr)
	{
		DefaultCameraBoomRelativeLocation = CameraBoom->GetRelativeLocation();
	}
	
	// 컴포넌트 값 확인
	if (MoveAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] MoveAction is NULL"), *GetActorLabel());
	if (LookAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] LookAction is NULL"), *GetActorLabel());
	if (JumpAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] JumpAction is NULL"), *GetActorLabel());
	if (RollAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] RollAction is NULL"), *GetActorLabel());
	if (ChangeViewAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] ChangeViewAction is NULL"), *GetActorLabel());
	if (ShootAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] ShootAction is NULL"), *GetActorLabel());
	if (SkillAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] SkillAction is NULL"), *GetActorLabel());
}

// Called every frame
void ALD8PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCrosshairSpread(DeltaTime);
}

// Called to bind functionality to input
void ALD8PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALD8PlayerCharacter::MoveInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ALD8PlayerCharacter::MoveInputCompleted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ALD8PlayerCharacter::MoveInputCompleted);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALD8PlayerCharacter::LookInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::RollInput);
		EnhancedInputComponent->BindAction(ChangeViewAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::ChangeView);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::ShootInput);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::Skill);
	}
}


/* ==================== Input ==================== */

void ALD8PlayerCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	DoMove(InputVector.X, InputVector.Y);
}

void ALD8PlayerCharacter::MoveInputCompleted(const FInputActionValue& Value)
{
	ClearMoveInput();
}

void ALD8PlayerCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ALD8PlayerCharacter::JumpInput()
{
	DoJumpStart();
}

void ALD8PlayerCharacter::RollInput()
{
	DoRoll();
}

void ALD8PlayerCharacter::ChangeView()
{
	bIsLeftView = !bIsLeftView;
	CameraBoom->SocketOffset = FVector(0.0f, bIsLeftView ? 75.0f : -75.0f, 0.0f);
}

void ALD8PlayerCharacter::ShootInput()
{
	if (Shoot(CurrentCrosshairSpread))
	{
		OnShotFired();
	}
}

void ALD8PlayerCharacter::Skill()
{
	if (SkillComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] SkillComponent is NULL"), *GetActorLabel());
		return;
	}

	SkillComponent->UseSkill();
}

/* ==================== Runtime ==================== */

void ALD8PlayerCharacter::OnShotFired()
{
	PlayFireCameraShake();
	AddShootCrosshairSpread();
}

void ALD8PlayerCharacter::PlayFireCameraShake()
{
	// 카메라 쉐이크가 설정되어 있지 않으면 종료
	if (FireCameraShake == nullptr)
	{
		return;
	}

	// 로컬 플레이어 컨트롤러가 아니면 종료
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr || !PC->IsLocalController())
	{
		return;
	}

	// 카메라 매니저가 없으면 종료
	if (PC->PlayerCameraManager == nullptr)
	{
		return;
	}

	// 카메라 쉐이크 실행
	PC->PlayerCameraManager->StartCameraShake(FireCameraShake, FireCameraShakeScale);
}

void ALD8PlayerCharacter::UpdateCrosshairSpread(float DeltaTime)
{
	const FVector Velocity = GetVelocity();
	const float GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();					// 지면 속도 계산 (X, Y 축만 고려)

	const bool bIsMoving = GroundSpeed > 10.0f;												// 이동 중인지 확인 (속도가 10 이상이면 이동 중)
	const float TargetSpread = bIsMoving ? MovementCrosshairSpread : MinCrosshairSpread;	// 이동 중이면 MovementCrosshairSpread, 아니면 MinCrosshairSpread

	// 크로스헤어 벌어짐 값을 보간하여 점진적으로 변경
	CurrentCrosshairSpread = FMath::FInterpTo(CurrentCrosshairSpread, TargetSpread, DeltaTime, CrosshairRecoverSpeed);

	// 크로스헤어 벌어짐 값을 최소/최대 값으로 제한
	CurrentCrosshairSpread = FMath::Clamp(CurrentCrosshairSpread, MinCrosshairSpread, MaxCrosshairSpread);
}

void ALD8PlayerCharacter::AddShootCrosshairSpread()
{
	// 발사 시 크로스헤어 벌어짐 값을 증가시키고 최소/최대 값으로 제한
	CurrentCrosshairSpread = FMath::Clamp(
		CurrentCrosshairSpread + ShootCrosshairSpreadAmount,
		MinCrosshairSpread,
		MaxCrosshairSpread
	);
}

float ALD8PlayerCharacter::GetCurrentCrosshairSpread() const
{
	return CurrentCrosshairSpread;
}

void ALD8PlayerCharacter::OnRollStarted(float CapsuleHeightDelta)
{
	Super::OnRollStarted(CapsuleHeightDelta);

	if (CameraBoom == nullptr)
		return;

	// 구르기 중 카메라 위치 보정
	FVector RollCameraLocation = DefaultCameraBoomRelativeLocation;
	RollCameraLocation.Z += CapsuleHeightDelta * RollCameraHeightCompensationScale;
	CameraBoom->SetRelativeLocation(RollCameraLocation);
}

void ALD8PlayerCharacter::OnRollEnded()
{
	Super::OnRollEnded();

	if (CameraBoom == nullptr)
		return;

	// 구르기 종료 후 카메라 위치를 기본 위치로 복원
	CameraBoom->SetRelativeLocation(DefaultCameraBoomRelativeLocation);
}
