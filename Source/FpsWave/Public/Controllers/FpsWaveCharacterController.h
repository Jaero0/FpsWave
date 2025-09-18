// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Characters/CharacterType.h"
#include "GameFramework/PlayerController.h"
#include "FpsWaveCharacterController.generated.h"

struct FInputActionValue;

DECLARE_DELEGATE(OnTpsFpsTypeChangedDelegate)
DECLARE_DELEGATE(OnFreeCameraStartedDelegate)
DECLARE_DELEGATE(OnFreeCameraCompletedDelegate)

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
	
	OnTpsFpsTypeChangedDelegate OnTpsFpsTypeChangedDelegate;
	OnFreeCameraStartedDelegate OnFreeCameraStartedDelegate;
	OnFreeCameraCompletedDelegate OnFreeCameraCompletedDelegate;

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
	void TpsFpsConversion();
	void Look(const FInputActionValue &InputActionValue);
	void LookFreeCameraStarted();
	void LookFreeCameraTriggered(const FInputActionValue& InputActionValue);
	void LookFreeCameraCompleted();

	TObjectPtr<class AFpsWaveCharacter> Player;

	UPROPERTY(VisibleAnywhere)
	float RunningSpeed = 2000.f;
	UPROPERTY(VisibleAnywhere)
	float WalkSpeed = 600.f;
	UPROPERTY(VisibleAnywhere)
	float CrouchSpeed = 30.f;

	FRotator FreeCameraStartedRotation;
	float CurrentFreeCamYaw = 0.f;
	float CurrentFreeCamPitch = 0.f;
	bool bIsFreeCamStarted = false;

	EToggleMode RunToggleMode = EToggleMode::ETM_None;
	EToggleMode CrouchToggleMode = EToggleMode::ETM_None;
	EMoveState CharacterMoveState = EMoveState::EMS_Walk;
	EPointOfViewType PointOfViewType = EPointOfViewType::EPT_ThirdPersonView;

public:
	FORCEINLINE
	EPointOfViewType GetPointOfViewType() const
	{
		return PointOfViewType;
	}
};
