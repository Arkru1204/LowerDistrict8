// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWERDISTRICT8_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/* 시작 시 호출 */
	virtual void NativeConstruct() override;

public:
	/* 체력 비율 반환 */
	UFUNCTION(BlueprintPure, Category = "UI|Health")
	float GetHealth() const;

private:
	UPROPERTY()
	class ALD8CharacterBase* PlayerPawn;
};
