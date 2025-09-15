// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Toggles.generated.h"

/**
 * 
 */
UCLASS()
class FPSWAVE_API UToggles : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UCheckBox> RunModeCheckBox;
};
