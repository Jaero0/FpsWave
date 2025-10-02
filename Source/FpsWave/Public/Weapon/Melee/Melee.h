// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/FpsWaveWeapon.h"
#include "Weapon/WieldableInterface.h"
#include "Melee.generated.h"

UCLASS()
class FPSWAVE_API AMelee : public AFpsWaveWeapon, public IWieldableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMelee();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//todo
	virtual float GetAttackDelay() override;
};
