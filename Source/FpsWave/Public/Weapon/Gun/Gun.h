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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Attack() override;
	void StartAutoFire();
	void FireSingleBullet();
	virtual void AttackFinished() override; 


private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USceneComponent> FirePoint;
	FTimerHandle AttackTimer;
	bool bIsFiring;
	float ReloadSpeed;
	int MaxBulletCount;
	int CurrentBulletCount;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABullet> Bullet;

public:
	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
