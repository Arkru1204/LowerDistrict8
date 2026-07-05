// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LD8Gun.generated.h"

UCLASS()
class LOWERDISTRICT8_API ALD8Gun : public AActor
{
	GENERATED_BODY()
	
	/* ==================== Gun Lifecycle ==================== */
public:	
	// Sets default values for this actor's properties
	ALD8Gun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	/* ==================== Properties ==================== */
private:
	UPROPERTY(EditAnywhere, Category = "Propertys|Asset")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Propertys|Asset")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Propertys|Asset")
	USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere, Category = "Propertys|Asset")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Propertys|Setting")
	float MaxRange = 5000;

	UPROPERTY(EditAnywhere, Category = "Propertys|Setting")
	float Damage = 10;

	UPROPERTY(EditAnywhere, Category = "Propertys|Setting")
	float FireInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Propertys|Projectile")
	TSubclassOf<class ALD8Projectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Propertys|Projectile")
	float AimRange = 5000.0f;


	/* ==================== Components ==================== */
private:
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Muzzle;


	/* ==================== Functions ==================== */
private:
	/* 히트스캔 총알이 맞았는지 여부 */
	//bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

	/* 투사체 발사 방향 반환 */
	FVector GetProjectileDirection() const;

	/* 총을 소유한 컨트롤러 반환 */
	AController* GetOwnerController() const;

public:
	/* 공격 시 호출되는 함수 */
	void PullTrigger();

private:
	/* 마지막 발사 시간 */
	float LastFireTime = -999.0f;
};
