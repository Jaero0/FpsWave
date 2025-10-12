// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Weapon/FpsWaveWeapon.h"
#include "Crosshair.generated.h"

/**
 * 
 */
enum class ECrosshairState
{
	ECS_Increasing,
	ECS_Idle,
	ECS_Decreasing
};

UCLASS()
class FPSWAVE_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Top;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Bottom;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Left;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Right;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UBorder> Aim;

	FVector2d DefaultTopLocation;
	FVector2d DefaultBottomLocation;
	FVector2d DefaultLeftLocation;
	FVector2d DefaultRightLocation;
	FVector2d CurrentTopLocation;
	FVector2d CurrentBottomLocation;
	FVector2d CurrentLeftLocation;
	FVector2d CurrentRightLocation;
	FVector2d PostTopLocation;
	FVector2d PostBottomLocation;
	FVector2d PostLeftLocation;
	FVector2d PostRightLocation;
	FVector2d MaxTopLocation;
	FVector2d MaxBottomLocation;
	FVector2d MaxLeftLocation;
	FVector2d MaxRightLocation;
	
	ECrosshairState CrosshairState = ECrosshairState::ECS_Idle;
	float LastIncreaseTime = 0.f;
	float AttackDelay = 0.f;

	float LimitAmount = 100.f;

	void BindDelegates();
	void ChangeDelegate(TObjectPtr<class AFpsWaveWeapon> Weapon);
	void UpdateCrosshairPositions();
	bool IsCurrentLocationNearToDefaultLocation();
	bool IsCurrentLocationNearToMaxLocation();

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FDelegateHandle CurrentDelegateHandle;

private:
	float AimIncreaseSpeed = 10.f;
	float AimDecreaseSpeed = 5.f;
	
	FTimerHandle DelegateHandle;
	void OnAttackDelegate();
	void ResetAimWidth();
	TObjectPtr<class AFpsWaveCharacterController> PlayerController;

	TObjectPtr<class AFpsWaveCharacter> Player;


public:
	FVector2d GetAimLocation();

	FORCEINLINE
	float GetAimIncreaseSpeed()
	{
		return AimIncreaseSpeed;
	}
	
	void SetAimIncreaseSpeed(float Speed);

	FORCEINLINE
	float GetAimDecreaseSpeed()
	{
		return AimDecreaseSpeed;
	}
	
	void SetAimDecreaseSpeed(float Speed);
};
