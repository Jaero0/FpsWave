// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsWave/Public/Characters/FpsWaveCharacter.h"


// Sets default values
AFpsWaveCharacter::AFpsWaveCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFpsWaveCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFpsWaveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFpsWaveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

