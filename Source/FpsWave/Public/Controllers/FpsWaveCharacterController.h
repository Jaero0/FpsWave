// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Characters/CharacterType.h"
#include "GameFramework/PlayerController.h"
#include "FpsWaveCharacterController.generated.h"

struct FInputActionValue;

DECLARE_DELEGATE(FOnTpsFpsTypeChangedDelegate)
DECLARE_DELEGATE(FOnFreeCameraStartedDelegate)
DECLARE_DELEGATE(FOnFreeCameraCompletedDelegate)
DECLARE_DELEGATE(FOnInteractionDelegate)
//todo int파라미터를 EKeyInput으로 교체하기
DECLARE_DELEGATE_OneParam(FOnWeaponChange_Key_Delegate, int)
DECLARE_DELEGATE_OneParam(FOnWeaponChange_MouseWheel_Delegate, int)
DECLARE_MULTICAST_DELEGATE(FOnAttackDelegate)
DECLARE_MULTICAST_DELEGATE(FOnAttackFinishedDelegate)
DECLARE_MULTICAST_DELEGATE(FOnReloadDelegate)

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
	
	FOnTpsFpsTypeChangedDelegate OnTpsFpsTypeChangedDelegate;
	FOnFreeCameraStartedDelegate OnFreeCameraStartedDelegate;
	FOnFreeCameraCompletedDelegate OnFreeCameraCompletedDelegate;
	FOnInteractionDelegate OnInteractionDelegate;
	FOnWeaponChange_Key_Delegate OnWeaponChange_Key_Delegate;
	FOnWeaponChange_MouseWheel_Delegate OnWeaponChange_MouseWheel_Delegate;
	FOnAttackDelegate OnAttackDelegate;
	FOnAttackFinishedDelegate OnAttackFinishedDelegate;
	FOnReloadDelegate OnReloadDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputDataAsset> InputDataAsset;
	

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ToggleWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> CrosshairWidget;
	TObjectPtr<class UCrosshair> CrosshairObj;
	
	TObjectPtr<class USpringArmComponent> GetCurrentSpringArm() const;
	void Move(const FInputActionValue &InputActionValue);
	void MoveEnd();
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
	EMoveState CharacterMoveState = EMoveState::EMS_Idle;
	EMoveState PrevCharacterMoveState = EMoveState::EMS_Idle;
	EPointOfViewType PointOfViewType = EPointOfViewType::EPT_ThirdPersonView;

	void Interact();

	void ChangeWeapon1();
	void ChangeWeapon2();
	void ChangeWeapon3();
	void ChangeWeapon4();
	void ChangeWeaponMouseWheel(const FInputActionValue &InputActionValue);

	void Attack();
	void AttackFinished();

	void Reload();

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

	TObjectPtr<UCrosshair> GetCrosshairObj();
};
