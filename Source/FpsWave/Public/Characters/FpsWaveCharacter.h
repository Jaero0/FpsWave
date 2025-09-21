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
	void HideHeadBone();

	void OnChangePointOfViewType();
	void OnFreeCameraStarted();
	void OnFreeCameraCompleted();

private:
#pragma region Camera
	TObjectPtr<class AFpsWaveCharacterController> FpsWaveController;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> TpsCameraChildActor;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> FpsSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> FpsCameraChildActor;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsZoomInSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> TpsZoomInCameraChildActor;

	FTimerHandle ViewTimerHandle;
	
	UPROPERTY(EditDefaultsOnly)
	float DefaultTpsSpringArmLength = 250.f;
	UPROPERTY(EditDefaultsOnly)
	float DefaultFpsSpringArmLength = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float DefaultTpsZoomInSpringArmLength = 100.f;

	bool bIsTpsZoomIn = false;

#pragma endregion

public:
	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetTpsSpringArm()
	{
		return TpsSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetFpsSpringArm()
	{
		return FpsSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetTpsCameraChildActor()
	{
		return TpsCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetFpsCameraChildActor()
	{
		return FpsCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetTpsZoomInCameraChildActor()
	{
		return TpsZoomInCameraChildActor;
	}

	FORCEINLINE
	bool GetIsTpsZoomIn() const
	{
		return bIsTpsZoomIn;
	}
};
