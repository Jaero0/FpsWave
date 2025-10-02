// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FpsWaveGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPSWAVE_API AFpsWaveGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ToggleWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> CrosshairWidget;

	virtual void BeginPlay() override;

	
};
