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
	TObjectPtr<class UNiagaraSystem> GunFireParticles;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class UParticleSystem> GunImpactParticles;
	UPROPERTY(EditAnywhere, Category="Gun")
	TObjectPtr<class USceneComponent> FirePoint;
	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere, Category="Gun")
	int32 MaxBulletCount;
	int32 CurrentBulletCount;
	
	bool bIsFiring;
	UPROPERTY(EditAnywhere, Category="Gun")
	float ReloadSpeed;
	
	UPROPERTY(EditAnywhere, Category="Gun")
	float MaxSpreadAngle;
	UPROPERTY(EditAnywhere, Category="Gun")
	float MinSpreadAngle;
	float AccuracyDecreasePerShot;
	float CurrentAccuracy;
	float MaxAccuracy;
	UPROPERTY(EditAnywhere, Category="Gun")
	float DefaultAccuracy;
	UPROPERTY(EditAnywhere, Category="Gun")
	float MinAccuracy;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Attack() override;
	virtual void StartAutoFire();
	virtual void FireSingleBullet();
	virtual void AttackFinished() override;
	void RecoverAccuracy(float DeltaTime);

UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilDuration = 0.15f; // 반동이 적용되는 시간
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilInterpSpeed = 10.0f; // 보간 속도

	UPROPERTY(EditAnywhere, Category="Recoil")
	float RecoilPitchMultiplier = 1.f;
	UPROPERTY(EditAnywhere, Category="Recoil")
	float RecoilYawMultiplier = 1.f;

	virtual void Recoil();

private:
	FRotator TargetRecoilRotation;
	FRotator CurrentRecoilRotation;
	float RecoilProgress;
	bool bIsRecoiling;

	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float GetAttackDelay() override;

	int32 GetMaxBulletCount();
	void ResetCurrentBulletCountToMax();

};
