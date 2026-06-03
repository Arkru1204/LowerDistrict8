// Fill out your copyright notice in the Description page of Project Settings.


#include "LD8AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LD8Character.h"

void ALD8AIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}

void ALD8AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALD8AIController::IsDead() const
{
	ALD8Character* ControlledCharacter = Cast<ALD8Character>(GetPawn()); // AI 캐릭터가 죽었는지 확인
	if (ControlledCharacter != nullptr)
	{
		return ControlledCharacter->IsDead();
	}

	return true; // 컨트롤러가 없어도 사망으로 간주
}
