// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FpsWaveCharacterCamera.generated.h"

UCLASS()
class FPSWAVE_API AFpsWaveCharacterCamera : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFpsWaveCharacterCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCameraComponent> Camera;
public:	

};
