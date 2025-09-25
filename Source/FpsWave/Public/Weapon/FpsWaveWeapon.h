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

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> ItemMesh;
	
	TObjectPtr<class AFpsWaveCharacter> Player;
	
	UFUNCTION()
	void OnCapsuleOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);
	
	UFUNCTION()
	void OnCapsuleOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:	
	FORCEINLINE
	TObjectPtr<class UStaticMeshComponent> GetItemMesh()
	{
		return ItemMesh;
	}

};
