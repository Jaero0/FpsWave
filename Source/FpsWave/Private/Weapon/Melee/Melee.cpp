// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Melee/Melee.h"


// Sets default values
AMelee::AMelee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentComboCount = 1;
}

// Called when the game starts or when spawned
void AMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMelee::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                            AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex,
                            bool bFromSweep,
                            const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMelee::Attack()
{
	if (bIsAttacking == true)
	{
		return;
	}
	if (OnAttackDelegate.IsBound())
	{
		OnAttackDelegate.Broadcast();
	}
	CurrentComboCount += 1;
}

void AMelee::AttackFinished()
{
	ResetComboCount();
}

void AMelee::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

int32 AMelee::GetCurrentComboCount()
{
	return CurrentComboCount;
}

float AMelee::GetAttackDelay()
{
	return AttackDelay;
}

void AMelee::ResetComboCount()
{
	CurrentComboCount = 1;
}

