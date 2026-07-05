// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Shoot.h"

#include "Kismet/GameplayStatics.h"
#include "AIController.h"

#include "LD8CharacterBase.h"
#include "LD8AIController.h"


UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = "Shoot";
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

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
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// AI의 현재 시점 위치 가져오기
	FVector ViewLocation;
	FRotator ViewRotation;
	AIController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// 플레이어 위치에서 Z만 살짝 올린 지점을 목표로 삼음
	FVector TargetLocation = PlayerPawn->GetActorLocation();
	TargetLocation.Z += AimHeightOffset;

	// 목표 위치를 바라보는 회전 계산
	FRotator OriginalRotation = AIController->GetControlRotation();
	FRotator ShootRotation = (TargetLocation - ViewLocation).Rotation();

	// Shoot 함수 실행
	AIController->SetControlRotation(ShootRotation);
	Character->Shoot();
	AIController->SetControlRotation(OriginalRotation);
	return EBTNodeResult::Succeeded;
}
