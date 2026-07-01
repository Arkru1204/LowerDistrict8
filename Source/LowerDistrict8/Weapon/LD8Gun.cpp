// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/LD8Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"


ALD8Gun::ALD8Gun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Muzzle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(Mesh);
}

void ALD8Gun::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALD8Gun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALD8Gun::PullTrigger()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Muzzle, TEXT("MuzzleFlashSocket"));	// 총구 섬광 스폰 후 붙이기
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Muzzle, TEXT("MuzzleFlashSocket"));	// 총구 사운드 스폰 후 붙이기

	FHitResult Hit;
	FVector ShotDirection;
	bool HitFlag = GunTrace(Hit, ShotDirection);
	if (HitFlag)
	{
		//DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Green, true);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());	// 피격 이펙트 스폰
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, Hit.Location);								// 피격 사운드 스폰

		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			//UE_LOG(LogTemp, Display, TEXT("%s"), *HitActor->GetActorLabel());
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			AController* OwnerController = GetOwnerController();
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}
}

bool ALD8Gun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr)
		return false;

	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	ShotDirection = -CameraRotation.Vector();

	FVector LineEnd = CameraLocation + CameraRotation.Vector() * MaxRange;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);		// 총 무시
	Params.AddIgnoredActor(GetOwner()); // 소유자 무시
	return GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, LineEnd, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

AController* ALD8Gun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return nullptr;
	return OwnerPawn->GetController();
}
