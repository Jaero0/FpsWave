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

	UPROPERTY(EditAnywhere, Category=Gun)
	int BulletPerShot;
	UPROPERTY(EditAnywhere, Category=Gun)
	float SpreadStartDistance;

	virtual void Attack() override;
	virtual void StartAutoFire() override;
	virtual void FireSingleBullet() override;
	virtual void AttackFinished() override;

	void Recoil();

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilDuration = 0.15f; // 반동이 적용되는 시간

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilInterpSpeed = 10.0f; // 보간 속도

private:
	FRotator TargetRecoilRotation;
	FRotator CurrentRecoilRotation;
	float RecoilProgress;
	bool bIsRecoiling;

public:
	virtual float GetAttackDelay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
