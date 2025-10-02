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
DECLARE_DELEGATE(OnInteractionDelegate)
DECLARE_DELEGATE_OneParam(OnWeaponChange_Key_Delegate, int)
DECLARE_DELEGATE_OneParam(OnWeaponChange_MouseWheel_Delegate, int)
DECLARE_DELEGATE(OnAttackDelegate)
DECLARE_DELEGATE(OnAttackFinishedDelegate)
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
	OnInteractionDelegate OnInteractionDelegate;
	OnWeaponChange_Key_Delegate OnWeaponChange_Key_Delegate;
	OnWeaponChange_MouseWheel_Delegate OnWeaponChange_MouseWheel_Delegate;
	OnAttackDelegate OnAttackDelegate;
	OnAttackFinishedDelegate OnAttackFinishedDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputDataAsset> InputDataAsset;
	

private:
	TObjectPtr<class USpringArmComponent> GetCurrentSpringArm() const;
	void Move(const FInputActionValue &InputActionValue);
	void RunInputCompleted(const FInputActionValue &InputActionValue);
	void RunInputStarted(const FInputActionValue &InputActionValue);
	void UpdateMoveSpeed();
	void CrouchInputStarted(const FInputActionValue &InputActionValue);
	void CrouchInputCompleted(const FInputActionValue &InputActionValue);
	void TpsFpsConversion();
	void Look(const FInputActionValue &InputActionValue);
	void LookFreeCameraStarted();
	void LookFreeCameraCompleted();
	void InterpolateFreeCamToOriginCam(float DeltaTime);
	void ZoomInStarted();
	void ZoomInCompleted();
	void ApplyCrouchState();

	TObjectPtr<class AFpsWaveCharacter> Player;

	UPROPERTY(EditAnywhere)
	float RunningSpeed = 1200.f;
	UPROPERTY(EditAnywhere)
	float WalkSpeed = 600.f;
	UPROPERTY(EditAnywhere)
	float ZoomInWalkSpeed = 350.f;

	FVector ForwardDirection;
	FVector RightDirection;

	float DefaultInterpolateTime = 0.1f;
	FRotator FreeCameraStartedRotation;
	float CurrentFreeCamYaw = 0.f;
	float CurrentFreeCamPitch = 0.f;
	UPROPERTY(VisibleAnywhere)
	bool bIsFreeCamStarted = false;
	bool bIsReturningFromFreeCam = false;
	UPROPERTY(EditDefaultsOnly)
	float CamReturningInterpSpeed = 200.f;
	bool bIsZoomIn = false;
	FVector CrouchStartedLocation;
	bool bIsCrouchState = false;

	EToggleMode RunToggleMode = EToggleMode::ETM_None;
	EToggleMode CrouchToggleMode = EToggleMode::ETM_None;
	EMoveState CharacterMoveState = EMoveState::EMS_Walk;
	EMoveState PrevCharacterMoveState = EMoveState::EMS_Walk;
	EPointOfViewType PointOfViewType = EPointOfViewType::EPT_ThirdPersonView;

	void Interact();

	void ChangeWeapon1();
	void ChangeWeapon2();
	void ChangeWeapon3();
	void ChangeWeapon4();
	void ChangeWeaponMouseWheel(const FInputActionValue &InputActionValue);

	void Attack();
	void AttackFinished();

public:
	FORCEINLINE
	EPointOfViewType GetPointOfViewType() const
	{
		return PointOfViewType;
	}

	UFUNCTION(BlueprintPure)
	EMoveState GetCharacterMoveState() const;

	UFUNCTION(BlueprintPure)
	bool GetIsFreeCamStarted() const;

	void SetCharacterMoveStateUpdateSpeed(EMoveState MoveState);
};
