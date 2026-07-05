// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_CombatAttack.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Kismet/GameplayStatics.h"

#include "LD8CharacterBase.h"


UBTTask_CombatAttack::UBTTask_CombatAttack()
{
	NodeName = "Combat Attack";

	bNotifyTick = true;			// TickTask를 사용하기 위해 bNotifyTick을 true로 설정
	bCreateNodeInstance = true;	// 인스턴스화된 노드로 설정하여 각 AI가 독립적인 상태를 가질 수 있도록 함
}

EBTNodeResult::Type UBTTask_CombatAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AI Controller가 없으면 실패
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// Pawn이 없으면 실패
	ALD8CharacterBase* Character = Cast<ALD8CharacterBase>(AIController->GetPawn());
	if (Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 플레이어 Pawn이 없으면 실패
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 이동을 멈추고 공격 시작
	AIController->StopMovement();

	ElapsedTime = 0.0f;				// 공격 지속 시간 초기화
	DirectionElapsedTime = 0.0f;	// 좌우 이동 방향 변경 시간 초기화
	ShootElapsedTime = 0.0f;		// 발사 간격 추적 초기화

	ChooseStrafeDirection();		// 초기 좌우 이동 방향 선택
	ChooseNextShootInterval();		// 초기 발사 간격 선택

	return EBTNodeResult::InProgress;
}

void UBTTask_CombatAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ALD8CharacterBase* Character = Cast<ALD8CharacterBase>(AIController->GetPawn());
	if (Character == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerComp.GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ElapsedTime += DeltaSeconds;			// 공격 지속 시간 추적
	DirectionElapsedTime += DeltaSeconds;	// 좌우 이동 방향 변경 시간 추적
	ShootElapsedTime += DeltaSeconds;		// 발사 간격 추적

	// 공격 지속 시간이 끝나면 태스크 종료
	if (ElapsedTime >= AttackDuration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 좌우 이동 방향 변경 간격이 지나면 새로운 방향 선택
	if (DirectionElapsedTime >= CurrentDirectionChangeInterval)
	{
		DirectionElapsedTime = 0.0f;
		ChooseStrafeDirection();
	}

	// 플레이어 위치 계산
	FVector TargetLocation = PlayerPawn->GetActorLocation();
	TargetLocation.Z += AimHeightOffset;

	// AI 시점에서 플레이어를 바라보는 회전 계산
	FVector ViewLocation;
	FRotator ViewRotation;
	AIController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FRotator ShootRotation = (TargetLocation - ViewLocation).Rotation();

	// 컨트롤러는 상하 조준까지 포함
	AIController->SetControlRotation(ShootRotation);

	// 캐릭터 몸은 Yaw만 사용해서 플레이어를 바라봄
	FRotator BodyRotation = FRotator(0.0f, ShootRotation.Yaw, 0.0f);
	Character->SetActorRotation(BodyRotation);

	// 좌우 이동
	if (!FMath::IsNearlyZero(CurrentStrafeDirection))
	{
		Character->AddMovementInput(Character->GetActorRightVector(), CurrentStrafeDirection * CurrentStrafeSpeedScale);
	}

	// Enemy 전용 발사 간격 체크
	if (ShootElapsedTime >= CurrentShootInterval)
	{
		ShootElapsedTime = 0.0f;

		Character->Shoot();

		ChooseNextShootInterval();
	}
}

void UBTTask_CombatAttack::ChooseStrafeDirection()
{
	CurrentDirectionChangeInterval = FMath::RandRange(DirectionChangeIntervalMin, DirectionChangeIntervalMax);
	CurrentStrafeSpeedScale = FMath::RandRange(StrafeSpeedScaleMin, StrafeSpeedScaleMax);

	const float RandomValue = FMath::FRand();
	if (RandomValue < StandStillChance)
	{
		CurrentStrafeDirection = 0.0f;
		return;
	}

	CurrentStrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;
}

void UBTTask_CombatAttack::ChooseNextShootInterval()
{
	const float MinInterval = FMath::Max(0.01f, FMath::Min(ShootIntervalMin, ShootIntervalMax));
	const float MaxInterval = FMath::Max(MinInterval, FMath::Max(ShootIntervalMin, ShootIntervalMax));

	CurrentShootInterval = FMath::RandRange(MinInterval, MaxInterval);
}
