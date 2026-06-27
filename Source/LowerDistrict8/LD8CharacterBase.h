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

	/* ==================== Properties ==================== */
protected:
	/* 최대 HP */
	UPROPERTY(EditDefaultsOnly)
	float MaxHP = 100;

	/* 현재 HP */
	UPROPERTY(VisibleAnywhere)
	float CurrentHP = MaxHP;

	/* 이동 속도 스케일 값 */
	UPROPERTY(EditAnywhere, Category = "Propertys|State")
	float MoveSpeedScale = 1.0f;	

	/* 점프력 스케일 값 */
	UPROPERTY(EditAnywhere, Category = "Propertys|State")
	float JumpPowerScale = 1.0f;


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


	/* ==================== Attack ==================== */
//protected:
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<AGun> GunClass; // Gun 클래스의 파생 클래스만 할당 가능

	//UPROPERTY()
	//AGun* Gun;
};
