// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"

/**
 * 
 */
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
	FVector2d MaxTopLocation;
	FVector2d MaxBottomLocation;
	FVector2d MaxLeftLocation;
	FVector2d MaxRightLocation;

	bool bAttackStarted = false;
	bool bAttackFinished = true;
	float MaxAttackDelay;
	float CurrentAttackDelay;

	void BindDelegates();
	void UpdateCrosshairPositions();

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	FTimerHandle DelegateHandle;
	void IncreaseAimWidth();
	void ResetAimWidth();

	TObjectPtr<class AFpsWaveCharacter> Player;
};
