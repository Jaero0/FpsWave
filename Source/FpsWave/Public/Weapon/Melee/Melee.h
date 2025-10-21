// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/FpsWaveWeapon.h"
#include "Weapon/WieldableInterface.h"
#include "Melee.generated.h"

UCLASS()
class FPSWAVE_API AMelee : public AFpsWaveWeapon, public IWieldableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMelee();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Attack() override;
	virtual void AttackFinished() override;

protected:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	UFUNCTION()
	virtual void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnHitBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	int32 CurrentComboCount = 1;

private:
	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> HitBox;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> TraceStartPoint;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> TraceEndPoint;

public:
	virtual int32 GetCurrentComboCount() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual float GetAttackDelay() override;
	virtual void ResetComboCount() override;
};
