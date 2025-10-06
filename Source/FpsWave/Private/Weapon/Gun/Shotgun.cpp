// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/Shotgun.h"


// Sets default values
AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackDelay = 0.7f;
	ReloadSpeed = 3.f;
	MaxBulletCount = 10;
	CurrentBulletCount = MaxBulletCount;
	MinAccuracy = 40;
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShotgun::Attack()
{
	Super::Attack();
}

void AShotgun::StartAutoFire()
{
	Super::StartAutoFire();
}

void AShotgun::FireSingleBullet()
{
	Super::FireSingleBullet();
}

void AShotgun::AttackFinished()
{
	Super::AttackFinished();
}

float AShotgun::GetAttackDelay()
{
	return AttackDelay;
}


// Called every frame
void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

