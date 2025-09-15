// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "FpsWaveCharacterController.generated.h"

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FPSWAVE_API AFpsWaveCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	void ChangeRunToggle();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	UPROPERTY(EditAnywhere)
	bool bIsRunToggleMode = false;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputDataAsset> InputDataAsset;

private:
	void Move(const FInputActionValue &InputActionValue);
	void RunInputCompleted(const FInputActionValue &InputActionValue);
	void RunInputStarted(const FInputActionValue &InputActionValue);
	void UpdateMoveSpeed();

	TObjectPtr<class AFpsWaveCharacter> Player;

	float RunningSpeed = 1200.f;
	float WalkSpeed = 600.f;

	
	bool bIsRunning = false;
};
