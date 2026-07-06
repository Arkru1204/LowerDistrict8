// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LD8SkillComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOWERDISTRICT8_API ULD8SkillComponent : public UActorComponent
{
	GENERATED_BODY()


	/* ==================== Skill Component Lifecycle ==================== */
public:	
	// Sets default values for this component's properties
	ULD8SkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	/* ==================== Properties ==================== */
private:
	/* 오버클럭 지속 시간 */
	UPROPERTY(EditAnywhere, Category = "Properties|Overclock")
	float OverclockDuration = 10.0f;

	/* 오버클럭 쿨타임 */
	UPROPERTY(EditAnywhere, Category = "Properties|Overclock")
	float OverclockCooldown = 5.0f;

	/* 오버클럭 중 월드 시간 배율 */
	UPROPERTY(EditAnywhere, Category = "Properties|Overclock")
	float OverclockWorldTimeDilation = 0.1f;

	/* 오버클럭 중 플레이어 시간 배율 */
	UPROPERTY(EditAnywhere, Category = "Properties|Overclock")
	float OverclockPlayerTimeScale= 0.2f;

	/* 오버클럭 활성화 여부 */
	UPROPERTY(VisibleAnywhere, Category = "Properties|Overclock")
	bool bIsOverclockActive = false;

	/* 스킬 사용 가능 여부 */
	UPROPERTY(VisibleAnywhere, Category = "Properties|Overclock")
	bool bCanUseSkill = true;


	/* ==================== Runtime Variables ==================== */
private:
	/* 오버클럭 경과 시간 */
	float OverclockElapsedTime = 0.0f;

	/* 쿨타임 경과 시간 */
	float CooldownElapsedTime = 0.0f;


	/* ==================== Functions ==================== */
public:
	/* 현재 스킬 사용 또는 중단 */
	void UseSkill();

	/* 스킬 사용 가능 여부 반환 */
	bool CanUseSkill() const;

private:
	/* 오버클럭 시작 */
	void BeginOverclock();

	/* 오버클럭 종료 */
	void EndOverclock();

	/* 시간 배율 초기화 */
	void ResetTimeDilation();


	/* ==================== Skill State Getters ==================== */
public:
	/* 오버클럭 지속시간 남은 비율 반환 */
	UFUNCTION(BlueprintPure, Category = "Skill|Overclock")
	float GetOverclockDurationPercent() const;

	/* 스킬 쿨타임 진행 비율 반환 */
	UFUNCTION(BlueprintPure, Category = "Skill|Overclock")
	float GetSkillCooldownPercent() const;

	/* 오버클럭 활성화 여부 반환 */
	UFUNCTION(BlueprintPure, Category = "Skill|Overclock")
	bool IsOverclockActive() const;

	/* 스킬 사용 가능 여부 반환 */
	UFUNCTION(BlueprintPure, Category = "Skill|Overclock")
	bool IsSkillReady() const;
};
