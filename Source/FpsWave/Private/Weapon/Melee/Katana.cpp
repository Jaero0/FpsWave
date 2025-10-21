// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Melee/Katana.h"


// Sets default values
AKatana::AKatana()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKatana::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKatana::Attack()
{
	Super::Attack();
}

void AKatana::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex,
                              bool bFromSweep,
                              const FHitResult& SweepResult)
{
	Super::OnHitBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

// Called every frame
void AKatana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

