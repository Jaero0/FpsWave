// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FpsWaveCharacter.generated.h"

UCLASS()
class FPSWAVE_API AFpsWaveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFpsWaveCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnChangePointOfViewType();
	void OnFreeCameraStarted();
	void OnFreeCameraCompleted();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsSpringArm;
	float FpsSpringArmLength = 0.f;
	float TpsSpringArmLength = 250.f;

	FRotator FreeCameraStartedRotation;
	float CurrentFreeCamYaw = 0.f;
	float CurrentFreeCamPitch = 0.f;
	bool bIsFreeCamStarted = false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> FpsCamera;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TpsCamera;

	TObjectPtr<class AFpsWaveCharacterController> FpsWaveController;

public:
	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetTpsSpringArm()
	{
		return TpsSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class UCameraComponent> GetFpsCamera()
	{
		return FpsCamera;
	}
};
