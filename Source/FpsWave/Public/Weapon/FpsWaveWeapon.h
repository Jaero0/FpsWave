// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FpsWaveWeapon.generated.h"

UCLASS()
class FPSWAVE_API AFpsWaveWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AFpsWaveWeapon();
	virtual void Tick(float DeltaTime) override;
	
	virtual void Attack();

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

	float AttackDelay;

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

};
