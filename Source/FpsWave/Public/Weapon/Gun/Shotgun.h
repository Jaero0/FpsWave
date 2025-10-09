// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Shotgun.generated.h"

UCLASS()
class FPSWAVE_API AShotgun : public AGun
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShotgun();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Shotgun")
	int BulletPerShot;
	UPROPERTY(EditAnywhere, Category="Shotgun")
	float SpreadStartDistance;

	virtual void Attack() override;
	virtual void StartAutoFire() override;
	virtual void FireSingleBullet() override;
	virtual void AttackFinished() override;

	virtual void Recoil() override;

private:

public:
	virtual float GetAttackDelay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
