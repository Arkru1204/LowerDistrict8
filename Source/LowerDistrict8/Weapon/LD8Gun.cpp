// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/LD8Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"

#include "Weapon/LD8Projectile.h"


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
	// 발사 간격 체크
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastFireTime < FireInterval)
	{
		return;
	}
	LastFireTime = CurrentTime;

	// 총구 섬광과 사운드 스폰
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Muzzle);
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Muzzle);

	// 투사체 클래스 체크
	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ProjectileClass is NULL"), *GetActorLabel());
		return;
	}

	// 총구 위치와 투사체 방향 계산
	FVector MuzzleLocation = Muzzle->GetComponentLocation();
	FVector ShootDirection = GetProjectileDirection();
	FRotator ProjectileRotation = ShootDirection.Rotation();

	// 투사체 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 투사체 스폰
	ALD8Projectile* Projectile = GetWorld()->SpawnActor<ALD8Projectile>(
		ProjectileClass,
		MuzzleLocation,
		ProjectileRotation,
		SpawnParams
	);

	// 투사체 초기화 호출
	if (Projectile != nullptr)
	{
		Projectile->InitProjectile(Damage, GetOwnerController());
	}
}

//bool ALD8Gun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
//{
//	AController* OwnerController = GetOwnerController();
//	if (OwnerController == nullptr)
//		return false;
//
//	FVector CameraLocation;
//	FRotator CameraRotation;
//	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
//	ShotDirection = -CameraRotation.Vector();
//
//	FVector LineEnd = CameraLocation + CameraRotation.Vector() * MaxRange;
//	FCollisionQueryParams Params;
//	Params.AddIgnoredActor(this);		// 총 무시
//	Params.AddIgnoredActor(GetOwner()); // 소유자 무시
//	return GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, LineEnd, ECollisionChannel::ECC_GameTraceChannel1, Params);
//}

FVector ALD8Gun::GetProjectileDirection() const
{
	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr)
	{
		return GetActorForwardVector(); // 소유자 컨트롤러가 없으면 총알이 날아갈 방향을 기본 전방 벡터로 설정
	}

	// 플레이어의 카메라 위치와 회전 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// 투사체가 날아갈 끝 지점 계산
	FVector AimEnd = CameraLocation + CameraRotation.Vector() * AimRange;

	// 라인 트레이스 파라미터 세팅
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);		// 총 무시
	Params.AddIgnoredActor(GetOwner()); // 소유자 무시

	// 라인 트레이스
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLocation,
		AimEnd,
		ECollisionChannel::ECC_GameTraceChannel1,
		Params
	);

	// 라인 트레이스 결과에 따라 날아갈 방향 결정
	FVector TargetLocation = bHit ? Hit.ImpactPoint : AimEnd; // 충돌 지점이 있으면 그 지점을 목표로, 없으면 AimEnd를 목표로 설정
	FVector MuzzleLocation = Muzzle->GetComponentLocation(); // 총구 위치 가져오기

	// 방향을 정규화하여 반환
	return (TargetLocation - MuzzleLocation).GetSafeNormal();
}

AController* ALD8Gun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (OwnerPawn == nullptr)
		return nullptr;

	return OwnerPawn->GetController();
}
