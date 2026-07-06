// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_RollEnd.h"

#include "Components/SkeletalMeshComponent.h"
#include "LD8CharacterBase.h"

void UAnimNotify_RollEnd::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
		return;

	ALD8CharacterBase* Character = Cast<ALD8CharacterBase>(MeshComp->GetOwner());
	if (Character == nullptr)
		return;

	Character->OnRollEndNotify();
}