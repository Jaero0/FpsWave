// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Melee.h"
#include "Katana.generated.h"

UCLASS()
class FPSWAVE_API AKatana : public AMelee
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKatana();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Attack() override;

protected:
	virtual void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
