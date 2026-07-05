// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LD8HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOWERDISTRICT8_API ULD8HUDWidget : public UUserWidget
{
	GENERATED_BODY()
	

	/* ==================== Lifecycle ==================== */
protected:
	/* 시작 시 호출 */
	virtual void NativeConstruct() override;


	/* ==================== Component ==================== */
private:
	/* 플레이어 캐릭터 참조 */
	UPROPERTY()
	class ALD8CharacterBase* PlayerPawn;

	/* 스킬 컴포넌트 참조 */
	UPROPERTY()
	class ULD8SkillComponent* SkillComponent;


	/* ==================== Health Functions ==================== */
public:
	/* 체력 비율 반환 */
	UFUNCTION(BlueprintPure, Category = "UI|Health")
	float GetHealth() const;


	/* ==================== Skill Functions ==================== */
public:
	/* 스킬 쿨타임 비율 반환 */
	UFUNCTION(BlueprintPure, Category = "UI|Skill")
	float GetSkillCooldownPercent() const;

	/* 스킬 지속시간 비율 반환 */
	UFUNCTION(BlueprintPure, Category = "UI|Skill")
	float GetSkillDurationPercent() const;

	/* 스킬 사용 가능 여부 반환 */
	UFUNCTION(BlueprintPure, Category = "UI|Skill")
	bool IsSkillReady() const;

	/* 스킬 활성화 여부 반환 */
	UFUNCTION(BlueprintPure, Category = "UI|Skill")
	bool IsSkillActive() const;

private:
	/* 스킬 컴포넌트 갱신 */
	void UpdateSkillComponent();

};
