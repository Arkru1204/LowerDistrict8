// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget.h"

#include "LD8CharacterBase.h"


void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPawn = Cast<ALD8CharacterBase>(GetOwningPlayerPawn());
}

float UHUDWidget::GetHealth() const
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
