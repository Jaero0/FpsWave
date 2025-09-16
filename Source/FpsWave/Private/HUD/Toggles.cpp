// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Toggles.h"

#include "Components/CheckBox.h"
#include "Controllers/FpsWaveCharacterController.h"

void UToggles::NativeConstruct()
{
	Super::NativeConstruct();

	if (RunModeCheckBox)
	{
		RunModeCheckBox->OnCheckStateChanged.AddDynamic(this, &UToggles::OnRunModeChanged);
		RunModeCheckBox->SetIsChecked(false);
	}

	if (CrouchModeCheckBox)
	{
		CrouchModeCheckBox->OnCheckStateChanged.AddDynamic(this, &UToggles::OnCrouchModeChanged);
		CrouchModeCheckBox->SetIsChecked(false);
	}
}

void UToggles::OnRunModeChanged(bool bIsChecked)
{
	if (auto* Controller = GetOwningPlayer<AFpsWaveCharacterController>())
	{
		Controller->OnChangeRunToggle(bIsChecked);
	}
}

void UToggles::OnCrouchModeChanged(bool bIsChecked)
{
	if (auto* Controller = GetOwningPlayer<AFpsWaveCharacterController>())
	{
		Controller->OnChangeCrouchToggle(bIsChecked);
	}
}
