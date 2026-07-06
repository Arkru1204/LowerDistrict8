// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "LD8CharacterBase.generated.h"

UCLASS()
class LOWERDISTRICT8_API ALD8CharacterBase : public ACharacter
{
	GENERATED_BODY()


	/* ==================== CharacterBase Lifecycle ==================== */
public:
	// Sets default values for this character's properties
	ALD8CharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* 데미지 처리 함수 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	/* ==================== Properties ==================== */
protected:
	/* 최대 HP */
	UPROPERTY(EditAnywhere, Category = "Properties|State")
	float MaxHP = 100;

	/* 현재 HP */
	UPROPERTY(EditAnywhere, Category = "Properties|State")
	float CurrentHP = MaxHP;

	/* 이동 속도 스케일 값 */
	UPROPERTY(EditAnywhere, Category = "Properties|State")
	float MoveSpeedScale = 1.0f;	

	/* 점프력 스케일 값 */
	UPROPERTY(EditAnywhere, Category = "Properties|State")
	float JumpPowerScale = 1.0f;

	/* 무기 소켓 이름 */
	UPROPERTY(EditAnywhere, Category = "Properties|Weapons")
	FName WeaponSocket = FName("HandGrip_R");

	/* 총 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "Properties|Weapons")
	TSubclassOf<class ALD8Gun> GunClass; // Gun 클래스의 파생 클래스만 할당 가능


	/* ==================== Roll Properties ==================== */
protected:
	/* 구르기 애니메이션 몽타주 */
	UPROPERTY(EditAnywhere, Category = "Properties|Roll")
	class UAnimMontage* RollMontage;

	/* 구르기 쿨타임 */
	UPROPERTY(EditAnywhere, Category = "Properties|Roll")
	float RollCooldown = 0.2f;

	/* 구르기 중 캡슐 절반 높이 */
	UPROPERTY(EditAnywhere, Category = "Properties|Roll")
	float RollCapsuleHalfHeight = 44.0f;

	/* 구르기 루트모션 이동 거리 배율 */
	UPROPERTY(EditAnywhere, Category = "Properties|Roll")
	float RollRootMotionDistanceScale = 1.6f;

	/* 구르기 중인지 */
	UPROPERTY(VisibleAnywhere, Category = "Properties|Roll")
	bool bIsRolling = false;

	/* 구르기 가능 여부 */
	UPROPERTY(VisibleAnywhere, Category = "Properties|Roll")
	bool bCanRoll = true;

private:
	/* 마지막 이동 입력 */
	FVector2D LastMoveInput = FVector2D::ZeroVector;

	/* 기본 컨트롤러 Yaw 회전 사용 여부 */
	bool DefaultUseControllerRotationYaw = false;
	/* 기본 이동 방향 회전 여부 */
	bool DefaultOrientRotationToMovement = false;
	/* 기본 컨트롤러 희망 회전 사용 여부 */
	bool DefaultUseControllerDesiredRotation = false;

	/* 기본 캡슐 절반 높이 */
	float DefaultCapsuleHalfHeight = 0.0f;
	/* 기본 메시 상대 위치 */
	FVector DefaultMeshRelativeLocation = FVector::ZeroVector;
	/* 현재 구르기 캡슐 높이 보정값 */
	float CurrentRollCapsuleHeightDelta = 0.0f;

	/* 구르기 쿨타임 타이머 */
	FTimerHandle RollCooldownTimerHandle;

public:
	/* Roll Notify에서 호출 */
	UFUNCTION(BlueprintCallable, Category = "Roll")
	void OnRollEndNotify();


	/* ==================== Movement ==================== */
protected:
	/* 실제 이동 처리 함수 */
	void DoMove(float Right, float Forward);
	/* 실제 카메라 회전 처리 함수 */
	void DoLook(float Yaw, float Pitch);
	/* Jump InputAction이 Triggered 될 때 호출되는 함수 */
	void DoJumpStart();
	/* 점프 입력이 끝났을 때 호출되는 함수 */
	void DoJumpEnd();
	/* 이동 입력 초기화 */
	void ClearMoveInput();

protected:
	/* 실제 구르기 처리 함수 */
	void DoRoll();
	/* 구르기 시작 시 호출 */
	virtual void OnRollStarted(float CapsuleHeightDelta);
	/* 구르기 종료 시 호출 */
	virtual void OnRollEnded();
private:
	/* 구르기 종료 */
	void EndRoll();
	/* 구르기 쿨타임 종료 */
	void ResetRollCooldown();
	/* 구르기 방향 계산 */
	FVector GetRollDirection() const;


	/* ==================== Attack ==================== */
protected:
	/* 총 클래스 */
	UPROPERTY()
	class ALD8Gun* Gun;
protected:
	/* 총 생성 함수 */
	void SpawnGun();
public:
	/* 실제 공격 처리 함수 */
	bool Shoot(float SpreadAngle = 0.0f);


	/* ==================== Getter ==================== */
public:
	/* 현재 HP가 0인지 확인 */
	UFUNCTION(BlueprintPure)
	bool IsDead() const;
	/* 현재 HP 비율 반환 */
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;
public:
	/* 구르기 중인지 확인 */
	UFUNCTION(BlueprintPure)
	bool IsRolling() const;
};
