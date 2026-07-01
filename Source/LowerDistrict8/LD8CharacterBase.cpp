// Fill out your copyright notice in the Description page of Project Settings.


#include "LD8CharacterBase.h"

#include "InputActionValue.h"

#include "Weapon/LD8Gun.h"


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

void ALD8CharacterBase::Shoot()
{
	Gun->PullTrigger();
}
