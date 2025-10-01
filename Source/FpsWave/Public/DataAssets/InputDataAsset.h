// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FPSWAVE_API UInputDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> RunAction;
	
	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> FpsTpsConversionAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> FreeCameraAction;
	
	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> ZoomInAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> InteractionAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> WeaponChangeAction1;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> WeaponChangeAction2;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> WeaponChangeAction3;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> WeaponChangeAction4;
	
	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> MouseWheelAction;

	UPROPERTY(EditAnywhere, Category="Input Asset")
	TObjectPtr<class UInputAction> AttackAction;
};
