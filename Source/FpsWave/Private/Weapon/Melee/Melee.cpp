// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Melee/Melee.h"


// Sets default values
AMelee::AMelee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

