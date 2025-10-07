// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/Rifle.h"


// Sets default values
ARifle::ARifle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AttackDelay = 0.1f;
	ReloadSpeed = 2.f;
	MaxBulletCount = 30;
	MaxAccuracy = 100.f;
	MinAccuracy = 60;
	CurrentAccuracy = MaxAccuracy;
	MaxSpreadAngle = 20.f;
	MinSpreadAngle = 0.5f;
	AccuracyDecreasePerShot = (MaxAccuracy - MinAccuracy) / (MaxBulletCount - 8);
}

// Called when the game starts or when spawned
void ARifle::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARifle::Attack()
{
	Super::Attack();
}

void ARifle::StartAutoFire()
{
	Super::StartAutoFire();
}

void ARifle::FireSingleBullet()
{
	Super::FireSingleBullet();
}

void ARifle::AttackFinished()
{
	Super::AttackFinished();
}

float ARifle::GetAttackDelay()
{
	return AttackDelay;
}

// Called every frame
void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



