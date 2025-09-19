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
#pragma region Camera
	TObjectPtr<class AFpsWaveCharacterController> FpsWaveController;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsSpringArm;
	float CurrentTargetArmLength = 0.f;
	UPROPERTY(EditAnywhere)
	float MaxTpsSpringArmLength = 250.f;
	UPROPERTY(EditAnywhere)
	float MinTpsSpringArmLength = 0.f;
	UPROPERTY(EditAnywhere)
	FRotator DefaultTpsSpringArmRot = FRotator(-15.f, 0.f, 0.f);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> FpsCamera;
	UPROPERTY(EditAnywhere)
	FRotator DefaultFpsCameraRot = FRotator(0.f, 90.f, -90.f);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TpsCamera;

	UPROPERTY(EditAnywhere)
	float CameraConversionInterpSpeed = 20.f;
	bool bIsCameraConversionTriggered = false;
#pragma endregion

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
