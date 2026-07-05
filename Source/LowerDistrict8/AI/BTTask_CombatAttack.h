// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CombatAttack.generated.h"

/**
 * 
 */
UCLASS()
class LOWERDISTRICT8_API UBTTask_CombatAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_CombatAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	/* 한 번 공격 패턴을 유지하는 시간 */
	UPROPERTY(EditAnywhere, Category = "Properties|Attack")
	float AttackDuration = 1.5f;

	/* 플레이어를 조준할 때 높이 보정 */
	UPROPERTY(EditAnywhere, Category = "Properties|Attack")
	float AimHeightOffset = 70.0f;

	/* Enemy가 발사 시도하는 최소 간격 */
	UPROPERTY(EditAnywhere, Category = "Properties|Attack")
	float ShootIntervalMin = 0.15f;

	/* Enemy가 발사 시도하는 최대 간격 */
	UPROPERTY(EditAnywhere, Category = "Properties|Attack")
	float ShootIntervalMax = 0.35f;

	/* 가만히 서서 공격할 확률 */
	UPROPERTY(EditAnywhere, Category = "Properties|Move")
	float StandStillChance = 0.35f;

	/* 좌우 이동 방향을 바꾸는 최소 간격 */
	UPROPERTY(EditAnywhere, Category = "Properties|Move")
	float DirectionChangeIntervalMin = 0.5f;

	/* 좌우 이동 방향을 바꾸는 최대 간격 */
	UPROPERTY(EditAnywhere, Category = "Properties|Move")
	float DirectionChangeIntervalMax = 0.9f;

	/* 좌우 이동 최소 속도 배율 */
	UPROPERTY(EditAnywhere, Category = "Properties|Move")
	float StrafeSpeedScaleMin = 0.5f;

	/* 좌우 이동 최대 속도 배율 */
	UPROPERTY(EditAnywhere, Category = "Properties|Move")
	float StrafeSpeedScaleMax = 0.9f;


private:
	/* 공격 지속 시간 추적 변수 */
	float ElapsedTime = 0.0f;

	/* 발사 간격 추적 변수 */
	float ShootElapsedTime = 0.0f;
	/* 현재 선택된 발사 간격 */
	float CurrentShootInterval = 0.3f;

	/* 좌우 이동 방향 변경 시간 추적 변수 */
	float DirectionElapsedTime = 0.0f;
	/* 현재 선택된 방향 변경 간격 */
	float CurrentDirectionChangeInterval = 0.7f;
	/* 현재 선택된 좌우 이동 속도 배율 */
	float CurrentStrafeSpeedScale = 0.7f;
	/* -1: 왼쪽, 0: 정지, 1: 오른쪽 */
	float CurrentStrafeDirection = 0.0f;

private:
	/* 좌우 이동 방향을 결정하는 함수 */
	void ChooseStrafeDirection();
	/* 다음 발사 간격을 랜덤으로 결정하는 함수 */
	void ChooseNextShootInterval();
};
