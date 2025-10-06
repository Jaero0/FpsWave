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
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class USceneComponent> FirePoint;
	FTimerHandle AttackTimer;
	FTimerHandle AccuracyRecoveryTimer;
	bool bIsFiring;
	UPROPERTY(EditAnywhere, Category="Gun")
	float ReloadSpeed;
	UPROPERTY(EditAnywhere, Category="Gun")
	int MaxBulletCount;
	int CurrentBulletCount;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class UNiagaraSystem> GunFireParticles;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class UParticleSystem> GunImpactParticles;
	float AccuracyDecreasePerShot;
	float CurrentAccuracy;
	float MaxAccuracy;
	float MinAccuracy;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Attack() override;
	virtual void StartAutoFire();
	virtual void FireSingleBullet();
	virtual void AttackFinished() override;
	void RecoverAccuracy();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float GetAttackDelay() override;

};
