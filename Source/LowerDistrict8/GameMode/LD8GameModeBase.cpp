// Fill out your copyright notice in the Description page of Project Settings.


#include "LD8GameModeBase.h"
#include "EngineUtils.h"
#include "AI/LD8AIController.h"


void ALD8GameModeBase::PawnKilled(APawn* PawnKilled)
{
	//UE_LOG(LogTemp, Warning, TEXT("A Pawn was killed!"));

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController()); // 플레이어가 죽었을 때
	if (PlayerController != nullptr)
	{
		//PlayerController->GameHasEnded(PlayerController->GetPawn(), false);
		EndGame(false);
	}

	for (ALD8AIController* Controller : TActorRange<ALD8AIController>(GetWorld())) // 월드 내의 AI 컨트롤러들
	{
		if (!Controller->IsDead()) // 살아있는 AI가 있으면 리턴
		{
			return;
		}
	}

	EndGame(true); // 모든 AI가 죽었을 때
}

void ALD8GameModeBase::EndGame(bool bIsPlayerWinner)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner; // 플레이어 컨트롤러인지
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}
