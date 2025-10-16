// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FpsWaveWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackDelegate);

UCLASS()
class FPSWAVE_API AFpsWaveWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AFpsWaveWeapon();
	virtual void Tick(float DeltaTime) override;

	FOnAttackDelegate OnAttackDelegate;
	
	virtual void Attack();
	virtual void AttackFinished();

protected:
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

	UPROPERTY(EditAnywhere, Category="Gun")
	int32 MaxBulletCount;
	int32 CurrentBulletCount;
	UPROPERTY(EditAnywhere, Category="Gun")
	float AttackDelay;
	int32 CurrentComboCount = 0;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> ItemMesh;


public:	
	FORCEINLINE
	TObjectPtr<class UStaticMeshComponent> GetItemMesh()
	{
		return ItemMesh;
	}
	
	FORCEINLINE
	TObjectPtr<class UBoxComponent> GetBoxComponent()
	{
		return BoxComponent;
	}
	
	virtual float GetAttackDelay();
	virtual int32 GetMaxBulletCount();
	virtual int32 GetCurrentBulletCount();

	virtual int32 GetCurrentComboCount();
	virtual void ResetComboCount();
};
