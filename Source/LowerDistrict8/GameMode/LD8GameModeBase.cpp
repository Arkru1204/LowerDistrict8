// Fill out your copyright notice in the Description page of Project Settings.


#include "LD8GameModeBase.h"

void ALD8GameModeBase::PawnKilled(APawn* PawnKilled)
{
	//UE_LOG(LogTemp, Warning, TEXT("A Pawn was killed!"));

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController()); // 플레이어가 죽었을 때
	if (PlayerController != nullptr)
	{
		PlayerController->GameHasEnded(PlayerController->GetPawn(), false);
	}
}
