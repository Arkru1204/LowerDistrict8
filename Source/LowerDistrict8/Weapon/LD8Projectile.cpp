// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/LD8Projectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"


/* ==================== Projectile Lifecycle ==================== */

ALD8Projectile::ALD8Projectile()
{
	PrimaryActorTick.bCanEverTick = false; // 매 프레임마다 Tick을 호출할 필요가 없으므로 false로 설정

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->InitSphereRadius(5.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);				// 충돌 활성화
	Collision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);			// 충돌 채널 설정
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);	// 모든 채널에 대해 충돌 응답을 Block으로 설정
	Collision->SetGenerateOverlapEvents(false);										// 오버랩 이벤트 비활성화
	Collision->SetNotifyRigidBodyCollision(true);									// 충돌 이벤트 알림 활성화

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;							// 투사체가 이동 방향을 따라 회전하도록 설정
	ProjectileMovement->bShouldBounce = false;										// 투사체가 충돌 시 튕기지 않도록 설정
	ProjectileMovement->ProjectileGravityScale = 0.0f;								// 투사체에 중력 영향을 받지 않도록 설정
}

void ALD8Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	Collision->OnComponentHit.AddDynamic(this, &ALD8Projectile::OnHit); // 충돌 이벤트에 대한 콜백 함수 등록

	if (GetOwner() != nullptr)
	{
		Collision->IgnoreActorWhenMoving(GetOwner(), true); // 투사체를 발사한 액터와의 충돌 무시
	}

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;

	SetLifeSpan(LifeTime); // 투사체가 일정 시간 후에 자동으로 제거되도록 설정
}

void ALD8Projectile::InitProjectile(float InDamage, AController* InOwnerController)
{
	Damage = InDamage;
	OwnerController = InOwnerController;

	if (OwnerController != nullptr)
	{
		SetInstigator(OwnerController->GetPawn()); // 투사체의 Instigator를 발사한 컨트롤러의 Pawn으로 설정
	}
}

void ALD8Projectile::OnHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (OtherActor == nullptr || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	// 충돌 시 이펙트와 사운드 재생
	if (ImpactEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}

	// 충돌 시 사운드 재생
	if (ImpactSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, Hit.ImpactPoint);
	}

	// 투사체끼리 충돌하면 둘 다 파괴
	if (ALD8Projectile* OtherProjectile = Cast<ALD8Projectile>(OtherActor))
	{
		OtherProjectile->Destroy();
		Destroy();
		return;
	}

	// 데미지 이벤트 생성 및 적용
	FPointDamageEvent DamageEvent(Damage, Hit, GetActorForwardVector(), nullptr);
	OtherActor->TakeDamage(Damage, DamageEvent, OwnerController, this);

	// 투사체 제거
	Destroy();
}
