// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LD8PlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"


ALD8PlayerController::ALD8PlayerController()
{

}

void ALD8PlayerController::BeginPlay()
{
	Super::BeginPlay();


	if (DefaultMappingContexts.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] DefaultMappingContexts is NULL"), *GetActorLabel());
	}

	// HUD 생성
	if (IsLocalPlayerController()) // 로컬 플레이어 컨트롤러에서만
	{
		if (HUDWidgetClass != nullptr)
		{
			HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);

			if (HUDWidget != nullptr)
			{
				HUDWidget->AddToViewport();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] HUDWidgetClass is NULL"), *GetActorLabel());
		}
	}
}

void ALD8PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 로컬 플레이어 컨트롤러용 IMC만 추가
	if (IsLocalPlayerController())
	{
		// 입력 매핑 컨텍스트 추가
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void ALD8PlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUDWidget->RemoveFromViewport();
	//if (bIsWinner)
	//{
	//	UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
	//	if (WinScreen != nullptr)
	//	{
	//		WinScreen->AddToViewport();
	//	}
	//}
	//else
	//{
	//	UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
	//	if (LoseScreen != nullptr)
	//	{
	//		LoseScreen->AddToViewport();
	//	}
	//}

	FTimerHandle RestartTimer;
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, 5.0f);
}
