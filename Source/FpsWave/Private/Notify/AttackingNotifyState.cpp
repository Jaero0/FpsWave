// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AttackingNotifyState.h"

#include "Characters/FpsWaveCharacter.h"

void UAttackingNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                        UAnimSequenceBase* Animation,
                                        float TotalDuration,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration ,EventReference);
	
	if (AFpsWaveCharacter* Char = Cast<AFpsWaveCharacter>(MeshComp->GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("bIsAttacking: true"));
		Char->SetIsAttacking(true);
	}
		
}

void UAttackingNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AFpsWaveCharacter* Char = Cast<AFpsWaveCharacter>(MeshComp->GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("bIsAttacking: false"));
		Char->SetQueuedCombo(false);
		Char->SetIsAttacking(false);
	}
}