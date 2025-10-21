// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ComboWindowNotifyState.h"

#include "Characters/FpsWaveCharacter.h"
#include "Weapon/FpsWaveWeapon.h"

void UComboWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                          UAnimSequenceBase* Animation,
                                          float TotalDuration,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AFpsWaveCharacter* Char = Cast<AFpsWaveCharacter>(MeshComp->GetOwner()))
	{
		Char->SetCanCombo(true);
	}
}

void UComboWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
                                        UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);


	if (AFpsWaveCharacter* Char = Cast<AFpsWaveCharacter>(MeshComp->GetOwner()))
	{
		if (Char->GetQueuedCombo() == true)
		{
			Char->GetEquippedWeapon()->Attack();
		}
		Char->SetCanCombo(false);
		Char->SetQueuedCombo(false);
	}
}
