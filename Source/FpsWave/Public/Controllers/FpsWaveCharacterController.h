// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Characters/CharacterType.h"
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
	UFUNCTION()
	void OnChangeRunToggle(bool bIsChecked);
	UFUNCTION()
	void OnChangeCrouchToggle(bool bIsChecked);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputDataAsset> InputDataAsset;

private:
	void Move(const FInputActionValue &InputActionValue);
	void RunInputCompleted(const FInputActionValue &InputActionValue);
	void RunInputStarted(const FInputActionValue &InputActionValue);
	void UpdateMoveSpeed();
	void CrouchInputStarted(const FInputActionValue &InputActionValue);
	void CrouchInputCompleted(const FInputActionValue &InputActionValue);

	TObjectPtr<class AFpsWaveCharacter> Player;

	UPROPERTY(VisibleAnywhere)
	float RunningSpeed = 2000.f;
	UPROPERTY(VisibleAnywhere)
	float WalkSpeed = 600.f;
	UPROPERTY(VisibleAnywhere)
	float CrouchSpeed = 30.f;

	EToggleMode RunToggleMode = EToggleMode::ETM_None;
	EToggleMode CrouchToggleMode = EToggleMode::ETM_None;
	EMoveState CharacterMoveState = EMoveState::EMS_Walk;
};
