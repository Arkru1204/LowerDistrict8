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
	CameraBoom->SetRelativeLocation(FVector(0.0f, 75.0f, 75.0f)); // 캐릭터 머리 위쪽에 위치
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 회전에 따라 회전

	// 팔로우 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라가 컨트롤러 회전에 따라 회전하지 않음

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
	
	// 컴포넌트 값 확인
	if (MoveAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] MoveAction is NULL"), *GetActorLabel());
	if (LookAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] LookAction is NULL"), *GetActorLabel());
	if (JumpAction == NULL)
		UE_LOG(LogTemp, Warning, TEXT("[%s] JumpAction is NULL"), *GetActorLabel());
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

}

// Called to bind functionality to input
void ALD8PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALD8PlayerCharacter::MoveInput);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALD8PlayerCharacter::LookInput);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::DoJumpStart);

		// Change View
		EnhancedInputComponent->BindAction(ChangeViewAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::ChangeView);

		// Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::ShootInput);

		// Skill
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &ALD8PlayerCharacter::Skill);
	}
}


/* ==================== Input ==================== */

void ALD8PlayerCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	DoMove(InputVector.X, InputVector.Y);
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

void ALD8PlayerCharacter::ChangeView()
{
	bIsLeftView = !bIsLeftView;
	CameraBoom->SetRelativeLocation(FVector(0.0f, bIsLeftView ? 75.0f : -75.0f, 75.0f));
}

void ALD8PlayerCharacter::ShootInput()
{
	if (Shoot())
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
