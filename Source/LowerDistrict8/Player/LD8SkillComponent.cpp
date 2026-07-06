// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LD8SkillComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"


/* ==================== Skill Component Lifecycle ==================== */

ULD8SkillComponent::ULD8SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULD8SkillComponent::BeginPlay()
{
	Super::BeginPlay();

}

void ULD8SkillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetTimeDilation();

	Super::EndPlay(EndPlayReason);
}

void ULD8SkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// GlobalTimeDilation 영향을 받지 않는 실제 시간 기준
	const float RealDeltaTime = FApp::GetDeltaTime();

	// 오버클럭 지속 시간 경과 체크
	if (bIsOverclockActive)
	{
		OverclockElapsedTime += RealDeltaTime;

		if (OverclockElapsedTime >= OverclockDuration)
		{
			EndOverclock();
		}

		return;
	}

	// 쿨타임 경과 체크
	if (!bCanUseSkill)
	{
		CooldownElapsedTime += RealDeltaTime;

		if (CooldownElapsedTime >= OverclockCooldown)
		{
			bCanUseSkill = true;
			CooldownElapsedTime = 0.0f;

			UE_LOG(LogTemp, Warning, TEXT("[%s] Skill Cooldown End"), *GetOwner()->GetActorLabel());
		}
	}
}


/* ==================== Functions ==================== */

void ULD8SkillComponent::UseSkill()
{
	if (bIsOverclockActive)
	{
		EndOverclock();
		return;
	}

	if (!CanUseSkill())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Skill is not ready"), *GetOwner()->GetActorLabel());
		return;
	}

	BeginOverclock();
}

bool ULD8SkillComponent::CanUseSkill() const
{
	return bCanUseSkill && !bIsOverclockActive;
}

void ULD8SkillComponent::BeginOverclock()
{
	AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
		return;

	// 안전하게 최소 시간 배율을 0.01로 설정
	const float SafeTimeDilation = FMath::Max(OverclockWorldTimeDilation, 0.01f);

	bIsOverclockActive = true;
	bCanUseSkill = false;
	OverclockElapsedTime = 0.0f;
	CooldownElapsedTime = 0.0f;

	// 월드 전체는 느리게
	UGameplayStatics::SetGlobalTimeDilation(this, SafeTimeDilation);

	// 플레이어 본인은 느려진 시간을 보정해서 상대적으로 빠르게 움직이게
	OwnerActor->CustomTimeDilation = OverclockPlayerTimeScale / SafeTimeDilation;

	UE_LOG(LogTemp, Warning, TEXT("[%s] Overclock Start"), *OwnerActor->GetActorLabel());
}

void ULD8SkillComponent::EndOverclock()
{
	AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
		return;

	ResetTimeDilation();

	bIsOverclockActive = false;
	OverclockElapsedTime = 0.0f;
	CooldownElapsedTime = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("[%s] Overclock End"), *OwnerActor->GetActorLabel());
}

void ULD8SkillComponent::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->CustomTimeDilation = 1.0f;
	}
}


/* ==================== Skill State Getters ==================== */

float ULD8SkillComponent::GetOverclockDurationPercent() const
{
	if (!bIsOverclockActive)
	{
		return 0.0f;
	}

	if (OverclockDuration <= 0.0f)
	{
		return 0.0f;
	}

	const float RemainingTime = OverclockDuration - OverclockElapsedTime;
	return FMath::Clamp(RemainingTime / OverclockDuration, 0.0f, 1.0f);
}

float ULD8SkillComponent::GetSkillCooldownPercent() const
{
	if (bCanUseSkill)
	{
		return 1.0f;
	}

	if (bIsOverclockActive)
	{
		return 0.0f;
	}

	if (OverclockCooldown <= 0.0f)
	{
		return 1.0f;
	}

	return FMath::Clamp(CooldownElapsedTime / OverclockCooldown, 0.0f, 1.0f);
}

bool ULD8SkillComponent::IsOverclockActive() const
{
	return bIsOverclockActive;
}

bool ULD8SkillComponent::IsSkillReady() const
{
	return bCanUseSkill;
}