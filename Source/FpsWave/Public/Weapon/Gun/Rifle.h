// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Rifle.generated.h"

UCLASS()
class FPSWAVE_API ARifle : public AGun
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARifle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Attack() override;
	virtual void StartAutoFire() override;
	virtual void FireSingleBullet() override;
	virtual void AttackFinished() override;

public:
	virtual float GetAttackDelay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
