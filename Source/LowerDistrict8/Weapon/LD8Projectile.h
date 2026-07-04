// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LD8Projectile.generated.h"

UCLASS()
class LOWERDISTRICT8_API ALD8Projectile : public AActor
{
	GENERATED_BODY()
	

	/* ==================== Projectile Lifecycle ==================== */
public:	
	// Sets default values for this actor's properties
	ALD8Projectile();

	/* 총에서 생성 직후 호출 */
	void InitProjectile(float InDamage, AController* InOwnerController);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	/* ==================== Components ==================== */
private:
	/* 충돌 담당 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* Collision;

	/* 총알 외형 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* Mesh;

	/* 투사체 이동 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;


	/* ==================== Properties ==================== */
private:
	/* 투사체가 가하는 데미지 */
	UPROPERTY(EditAnywhere, Category = "Properties|Setting")
	float Damage = 10.0f;

	/* 투사체가 사라지기까지 걸리는 시간 */
	UPROPERTY(EditAnywhere, Category = "Properties|Setting")
	float LifeTime = 5.0f;

	/* 투사체의 이동 속도 */
	UPROPERTY(EditAnywhere, Category = "Properties|Setting")
	float Speed = 6000.0f;

	/* 투사체가 충돌했을 때 발생하는 이펙트 */
	UPROPERTY(EditAnywhere, Category = "Properties|Asset")
	UParticleSystem* ImpactEffect;

	/* 투사체가 충돌했을 때 발생하는 사운드 */
	UPROPERTY(EditAnywhere, Category = "Properties|Asset")
	USoundBase* ImpactSound;

	/* 투사체를 발사한 컨트롤러 */
	UPROPERTY()
	AController* OwnerController;


	/* ==================== Functions ==================== */
private:
	/* 투사체가 충돌했을 때 호출되는 함수 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

};
