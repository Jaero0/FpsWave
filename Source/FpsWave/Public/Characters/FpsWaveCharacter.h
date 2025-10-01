// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterType.h"
#include "GameFramework/Character.h"
#include "FpsWaveCharacter.generated.h"

USTRUCT()
struct FDefaultWeapon
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGun> DefaultRifle;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGun> DefaultShotgun;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMelee> DefaultKatana;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMelee> DefaultHammer;

public:
FORCEINLINE
	TSubclassOf<class AGun> GetDefaultRifle()
	{
		return DefaultRifle;
	}

FORCEINLINE
	TSubclassOf<class AGun> GetDefaultShotgun()
	{
		return DefaultShotgun;
	}

FORCEINLINE
	TSubclassOf<class AMelee> GetDefaultKatana()
	{
		return DefaultKatana;
	}

FORCEINLINE
	TSubclassOf<class AMelee> GetDefaultHammer()
	{
		return DefaultHammer;
	}
};

USTRUCT(BlueprintType)
struct FWeaponInventory
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	FDefaultWeapon DefaultWeapon;

public:
	FORCEINLINE
	FDefaultWeapon GetDefaultWeapon()
	{
		return DefaultWeapon;
	}

public:
	TObjectPtr<AGun> AttachedRifle;
	TObjectPtr<AGun> AttachedShotgun;
	TObjectPtr<AMelee> AttachedKatana;
	TObjectPtr<AMelee> AttachedHammer;

	// 기타 필요한 무기 정보들 추가 가능
};

UCLASS()
class FPSWAVE_API AFpsWaveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFpsWaveCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ChangeCapsuleSizeCrouch(EMoveState MoveState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void HideHeadBone();

	void OnChangePointOfViewType();
	void OnFreeCameraStarted();
	void OnFreeCameraCompleted();

private:
	
#pragma region Camera
	TObjectPtr<class AFpsWaveCharacterController> FpsWaveController;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> TpsCameraChildActor;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsCrouchSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> TpsCrouchCameraChildActor;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> FpsSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> FpsCameraChildActor;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> FpsCrouchSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> FpsCrouchCameraChildActor;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> TpsZoomInSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChildActorComponent> TpsZoomInCameraChildActor;

	FTimerHandle ViewTimerHandle;
	
	UPROPERTY(EditDefaultsOnly)
	float DefaultTpsSpringArmLength = 250.f;
	UPROPERTY(EditDefaultsOnly)
	float DefaultFpsSpringArmLength = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float DefaultTpsZoomInSpringArmLength = 100.f;

#pragma endregion

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AFpsWaveWeapon> DetectedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FWeaponInventory WeaponInventory;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AFpsWaveWeapon> EquippedWeapon;
	UPROPERTY(VisibleAnywhere)
	EOverlapDetected OverlapDetectedType = EOverlapDetected::EOD_None;
	EPlayerEquipType EquipType = EPlayerEquipType::EPE_Gun;

	void Interact();

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAnimMontage> SwapWeaponMontage;
	void ChangeWeapon_Key(int key);
	int WeaponIndex = 1;
	void ChangeWeapon_MouseWheel(int input);

	void Attack();
	void AttackFinished();

public:
	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetTpsSpringArm()
	{
		return TpsSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetTpsCrouchSpringArm()
	{
		return TpsCrouchSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetFpsSpringArm()
	{
		return FpsSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class USpringArmComponent> GetFpsCrouchSpringArm()
	{
		return FpsCrouchSpringArm;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetTpsCameraChildActor()
	{
		return TpsCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetTpsCrouchCameraChildActor()
	{
		return TpsCrouchCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetFpsCameraChildActor()
	{
		return FpsCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetFpsCrouchCameraChildActor()
	{
		return FpsCrouchCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<class UChildActorComponent> GetTpsZoomInCameraChildActor()
	{
		return TpsZoomInCameraChildActor;
	}

	FORCEINLINE
	TObjectPtr<AFpsWaveWeapon> GetDetectedWeapon()
	{
		return DetectedWeapon;
	}

	FORCEINLINE
	void SetDetectedWeapon(AFpsWaveWeapon* Weapon)
	{
		this->DetectedWeapon = Weapon;
	}

	FORCEINLINE
	EOverlapDetected GetOverlapDetectedType()
	{
		return OverlapDetectedType;
	}

	FORCEINLINE
	void SetOverlapDetectedType(EOverlapDetected DetectedType)
	{
		this->OverlapDetectedType = DetectedType;
	}

	UFUNCTION(BlueprintPure)
	EPlayerEquipType GetPlayerEquipType() const;
};
