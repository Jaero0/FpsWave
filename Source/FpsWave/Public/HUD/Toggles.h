// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/CharacterType.h"
#include "Toggles.generated.h"

/**
 * 
 */
UCLASS()
class FPSWAVE_API UToggles : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnRunModeChanged(bool bIsChecked);
	UFUNCTION()
	void OnCrouchModeChanged(bool bIsChecked);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCheckBox> RunModeCheckBox;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCheckBox> CrouchModeCheckBox;
};
