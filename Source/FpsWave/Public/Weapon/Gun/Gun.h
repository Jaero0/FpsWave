// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/FpsWaveWeapon.h"
#include "Weapon/ShootableInterface.h"
#include "Gun.generated.h"




UCLASS()
class FPSWAVE_API AGun : public AFpsWaveWeapon, public IShootableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGun();

protected:
	TObjectPtr<class AFpsWaveCharacterController> PlayerController;

	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class USoundBase> FireSound;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class USceneComponent> FirePoint;
	FTimerHandle AttackTimer;
	int CurrentBulletCount;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class UNiagaraSystem> GunFireParticles;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class UParticleSystem> GunImpactParticles;
	bool bIsFiring;
	UPROPERTY(EditAnywhere, Category="Gun")
	float ReloadSpeed;
	UPROPERTY(EditAnywhere, Category="Gun")
	int MaxBulletCount;
	UPROPERTY(EditAnywhere, Category="Gun")
	float MaxSpreadAngle;
	UPROPERTY(EditAnywhere, Category="Gun")
	float MinSpreadAngle;
	float AccuracyDecreasePerShot;
	float CurrentAccuracy;
	UPROPERTY(EditAnywhere, Category="Gun")
	float MaxAccuracy;
	UPROPERTY(EditAnywhere, Category="Gun")
	float MinAccuracy;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Attack() override;
	virtual void StartAutoFire();
	virtual void FireSingleBullet();
	virtual void AttackFinished() override;
	void RecoverAccuracy(float DeltaTime);

private:
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float GetAttackDelay() override;

};
