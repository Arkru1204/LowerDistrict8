// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LD8GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LOWERDISTRICT8_API ALD8GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	/* 캐릭터가 죽었을 때 호출되는 함수 */
	void PawnKilled(APawn* PawnKilled);

private:
	void EndGame(bool bIsPlayerWinner);
};
