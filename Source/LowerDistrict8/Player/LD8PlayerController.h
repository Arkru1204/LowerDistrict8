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

public:
	/* 게임 종료 시 호출되는 함수 */
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

private:
	/* 입력 매핑 컨텍스트 */
	UPROPERTY(EditAnywhere, Category = "Components")
	TArray<class UInputMappingContext*> DefaultMappingContexts;

	/* HUD 위젯 클래스 */
	UPROPERTY(EditAnywhere, Category = "Properties|UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	/* 생성된 HUD 위젯 */
	UPROPERTY()
	class UUserWidget* HUDWidget;
};
