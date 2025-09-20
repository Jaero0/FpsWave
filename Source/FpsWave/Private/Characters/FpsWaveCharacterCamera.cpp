// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FpsWaveCharacterCamera.h"

#include "Camera/CameraComponent.h"

// Sets default values
AFpsWaveCharacterCamera::AFpsWaveCharacterCamera()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RootComponent = Camera;
}

// Called when the game starts or when spawned
void AFpsWaveCharacterCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

