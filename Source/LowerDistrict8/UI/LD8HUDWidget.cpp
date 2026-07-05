// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LD8HUDWidget.h"

#include "LD8CharacterBase.h"
#include "Player/LD8SkillComponent.h"


/* ==================== Lifecycle ==================== */

void ULD8HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPawn = Cast<ALD8CharacterBase>(GetOwningPlayerPawn());
	UpdateSkillComponent();
}


/* ==================== Health Functions ==================== */

float ULD8HUDWidget::GetHealth() const
{
	const ALD8CharacterBase* Character = PlayerPawn;

	// 위젯 생성 시점에 Pawn이 아직 없었거나, 나중에 Pawn이 바뀐 경우 대비
	if (Character == nullptr)
	{
		Character = Cast<ALD8CharacterBase>(GetOwningPlayerPawn());
	}

	if (Character == nullptr)
	{
		return 0.0f;
	}

	return Character->GetHealthPercent();
}


/* ==================== Skill Functions ==================== */

void ULD8HUDWidget::UpdateSkillComponent()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (OwningPawn == nullptr)
	{
		SkillComponent = nullptr;
		return;
	}

	SkillComponent = OwningPawn->FindComponentByClass<ULD8SkillComponent>();
}

float ULD8HUDWidget::GetSkillCooldownPercent() const
{
	return SkillComponent ? SkillComponent->GetSkillCooldownPercent() : 0.0f;
}

float ULD8HUDWidget::GetSkillDurationPercent() const
{
	return SkillComponent ? SkillComponent->GetOverclockDurationPercent() : 0.0f;
}

bool ULD8HUDWidget::IsSkillReady() const
{
	return SkillComponent ? SkillComponent->IsSkillReady() : false;
}

bool ULD8HUDWidget::IsSkillActive() const
{
	return SkillComponent ? SkillComponent->IsOverclockActive() : false;
}
