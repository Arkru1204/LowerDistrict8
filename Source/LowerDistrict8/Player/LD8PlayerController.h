// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "LD8PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOWERDISTRICT8_API ALD8PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALD8PlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* 입력 매핑 컨텍스트 설정 */
	virtual void SetupInputComponent() override;

private:
	/* 입력 매핑 컨텍스트 */
	UPROPERTY(EditAnywhere, Category = "Components")
	TArray<class UInputMappingContext*> DefaultMappingContexts;

};
