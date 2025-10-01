// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class FPSWAVE_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);
	
	UFUNCTION()
	virtual void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TObjectPtr<class UCapsuleComponent> GetCapsuleComponent();
};
