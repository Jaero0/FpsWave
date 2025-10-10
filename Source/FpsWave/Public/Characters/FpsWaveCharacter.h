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
	UPROPERTY(EditAnywhere, Category="DefaultWeapon")
	TSubclassOf<class ARifle> DefaultRifle;

	UPROPERTY(EditAnywhere, Category="DefaultWeapon")
	TSubclassOf<class AShotgun> DefaultShotgun;

	UPROPERTY(EditAnywhere, Category="DefaultWeapon")
	TSubclassOf<class AKatana> DefaultKatana;

	UPROPERTY(EditAnywhere, Category="DefaultWeapon")
	TSubclassOf<class AWarHammer> DefaultHammer;

public:
FORCEINLINE
	TSubclassOf<class ARifle> GetDefaultRifle()
	{
		return DefaultRifle;
	}

FORCEINLINE
	TSubclassOf<class AShotgun> GetDefaultShotgun()
	{
		return DefaultShotgun;
	}

FORCEINLINE
	TSubclassOf<class AKatana> GetDefaultKatana()
	{
		return DefaultKatana;
	}

FORCEINLINE
	TSubclassOf<class AWarHammer> GetDefaultHammer()
	{
		return DefaultHammer;
	}
};

USTRUCT(BlueprintType)
struct FWeaponInventory
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="DefaultWeapon")
	FDefaultWeapon DefaultWeapon;

public:
	FORCEINLINE
	FDefaultWeapon GetDefaultWeapon()
	{
		return DefaultWeapon;
	}

public:
	TObjectPtr<ARifle> AttachedRifle;
	TObjectPtr<AShotgun> AttachedShotgun;
	TObjectPtr<AKatana> AttachedKatana;
	TObjectPtr<AWarHammer> AttachedHammer;

	// 기타 필요한 무기 정보들 추가 가능
};

UCLASS()
class FPSWAVE_API AFpsWaveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFpsWaveCharacter();
	virtual void Tick(float DeltaTime) override;
	void ChangeCapsuleSizeCrouch(EMoveState MoveState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void HideHeadBone();

	void OnChangePointOfViewType();
	void OnFreeCameraStarted();
	void OnFreeCameraCompleted();

private:
	FTimerHandle DelegateTimer;
	void BindDelegate();
	
#pragma region Camera
	TObjectPtr<class AFpsWaveCharacterController> FpsWaveController;
	
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> TpsSpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UChildActorComponent> TpsCameraChildActor;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> TpsCrouchSpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UChildActorComponent> TpsCrouchCameraChildActor;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> FpsSpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UChildActorComponent> FpsCameraChildActor;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> FpsCrouchSpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UChildActorComponent> FpsCrouchCameraChildActor;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class USpringArmComponent> TpsZoomInSpringArm;
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<class UChildActorComponent> TpsZoomInCameraChildActor;

	FTimerHandle ViewTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float DefaultTpsSpringArmLength = 250.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float DefaultFpsSpringArmLength = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float DefaultTpsZoomInSpringArmLength = 100.f;

#pragma endregion

	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<class AFpsWaveWeapon> DetectedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Weapon")
	FWeaponInventory WeaponInventory;
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<class AFpsWaveWeapon> EquippedWeapon;
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	EOverlapDetected OverlapDetectedType = EOverlapDetected::EOD_None;
	EPlayerWeaponType WeaponEquipType = EPlayerWeaponType::EPW_Rifle;

	void Interact();

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<class UAnimMontage> SwapWeaponMontage;
	void ChangeWeapon_Key(int key);
	int WeaponIndex = 1;
	void ChangeWeapon_MouseWheel(int input);
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<class UAnimMontage> AttackMontage;
	EAttackState PlayerAttackState = EAttackState::EAS_None;
	void Attack();
	UFUNCTION()
	void PlayAttackMontage();
	void AttackFinished();

	UPROPERTY(EditAnywhere, Category="Reload");
	TObjectPtr<UAnimMontage> ReloadMontage;
	void Reload();
	UFUNCTION(BlueprintCallable)
	void ReloadEnd();

public:
	TObjectPtr<USpringArmComponent> GetTpsSpringArm();

	TObjectPtr<USpringArmComponent> GetTpsCrouchSpringArm();

	TObjectPtr<USpringArmComponent> GetFpsSpringArm();

	TObjectPtr<USpringArmComponent> GetFpsCrouchSpringArm();

	TObjectPtr<UChildActorComponent> GetTpsCameraChildActor();

	TObjectPtr<UChildActorComponent> GetTpsCrouchCameraChildActor();

	TObjectPtr<UChildActorComponent> GetFpsCameraChildActor();

	TObjectPtr<UChildActorComponent> GetFpsCrouchCameraChildActor();

	TObjectPtr<UChildActorComponent> GetTpsZoomInCameraChildActor();

	TObjectPtr<AFpsWaveWeapon> GetDetectedWeapon();

	void SetDetectedWeapon(AFpsWaveWeapon* Weapon);

	EOverlapDetected GetOverlapDetectedType() const;

	void SetOverlapDetectedType(EOverlapDetected DetectedType);

	UFUNCTION(BlueprintPure)
	EPlayerWeaponType GetPlayerWeaponType() const;

	UFUNCTION(BlueprintPure)
	EAttackState GetPlayerAttackState() const;

	TObjectPtr<class AFpsWaveWeapon> GetEquippedWeapon();
};
