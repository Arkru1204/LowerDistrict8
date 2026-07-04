// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Shoot.h"

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
	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// Pawn이 없으면 실패
	ALD8CharacterBase* Character = Cast<ALD8CharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// Shoot 함수 실행
	Character->Shoot();
	return EBTNodeResult::Succeeded;
}
