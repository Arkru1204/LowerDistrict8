// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LD8AIController.generated.h"

/**
 * 
 */
UCLASS()
class LOWERDISTRICT8_API ALD8AIController : public AAIController
{
	GENERATED_BODY()
	

	/* ==================== AIController Lifecycle ==================== */
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


	/* ==================== Properties ==================== */
private:
	UPROPERTY(EditAnywhere, Category = "Properties")
	class UBehaviorTree* AIBehavior;


	/* ==================== Functions ==================== */
public:
	bool IsDead() const;
};
