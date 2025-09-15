// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TogglesComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPSWAVE_API UTogglesComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UToggles> TogglesWidget;
};
