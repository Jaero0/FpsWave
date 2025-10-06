// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Melee/WarHammer.h"


// Sets default values
AWarHammer::AWarHammer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWarHammer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWarHammer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

