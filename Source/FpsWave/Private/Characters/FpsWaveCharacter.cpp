// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsWave/Public/Characters/FpsWaveCharacter.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/FpsWaveWeapon.h"
#include "Weapon/Gun/Gun.h"
#include "Weapon/Gun/Rifle.h"
#include "Weapon/Gun/Shotgun.h"
#include "Weapon/Melee/Katana.h"
#include "Weapon/Melee/Melee.h"
#include "Weapon/Melee/WarHammer.h"


// Sets default values
AFpsWaveCharacter::AFpsWaveCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	TpsSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TpsSpringArm"));
	TpsSpringArm->SetupAttachment(GetRootComponent());
	TpsSpringArm->TargetArmLength = DefaultTpsSpringArmLength;
	TpsCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("TpsCamera"));
	TpsCameraChildActor->SetupAttachment(TpsSpringArm);

	TpsCrouchSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TpsCrouchSpringArm"));
	TpsCrouchSpringArm->SetupAttachment(GetRootComponent());
	TpsCrouchSpringArm->TargetArmLength = DefaultTpsSpringArmLength;
	TpsCrouchCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("TpsCrouchCamera"));
	TpsCrouchCameraChildActor->SetupAttachment(TpsCrouchSpringArm);

	FpsSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FpsSpringArm"));
	FpsSpringArm->SetupAttachment(GetRootComponent());
	FpsSpringArm->TargetArmLength = DefaultFpsSpringArmLength;
	FpsCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("FpsCamera"));
	FpsCameraChildActor->SetupAttachment(FpsSpringArm);
	
	FpsCrouchSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FpsCrouchSpringArm"));
	FpsCrouchSpringArm->SetupAttachment(GetRootComponent());
	FpsCrouchSpringArm->TargetArmLength = DefaultFpsSpringArmLength;
	FpsCrouchCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("FpsCrouchCamera"));
	FpsCrouchCameraChildActor->SetupAttachment(FpsCrouchSpringArm);

	TpsZoomInSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TpsZoomInSpringArm"));
	TpsZoomInSpringArm->SetupAttachment(GetRootComponent());
	TpsZoomInSpringArm->TargetArmLength = DefaultTpsZoomInSpringArmLength;
	TpsZoomInCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("TpsZoomInCamera"));
	TpsZoomInCameraChildActor->SetupAttachment(TpsZoomInSpringArm);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = UE::Math::TRotator(0.0, 460.0, 0.0);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void AFpsWaveCharacter::BindDelegate()
{
	if (auto Cont = Cast<AFpsWaveCharacterController>(GetWorld()->GetFirstPlayerController()))
	{
		FpsWaveController = Cont;
		Cont->OnTpsFpsTypeChangedDelegate.BindUObject(this, &AFpsWaveCharacter::OnChangePointOfViewType);
		Cont->OnFreeCameraStartedDelegate.BindUObject(this, &AFpsWaveCharacter::OnFreeCameraStarted);
		Cont->OnFreeCameraCompletedDelegate.BindUObject(this, &AFpsWaveCharacter::OnFreeCameraCompleted);
		Cont->OnInteractionDelegate.BindUObject(this, &AFpsWaveCharacter::Interact);
		Cont->OnWeaponChange_Key_Delegate.BindUObject(this, &AFpsWaveCharacter::ChangeWeapon_Key);
		Cont->OnWeaponChange_MouseWheel_Delegate.BindUObject(this, &AFpsWaveCharacter::ChangeWeapon_MouseWheel);
		Cont->OnAttackDelegate.AddUObject(this, &AFpsWaveCharacter::Attack);
		Cont->OnAttackFinishedDelegate.AddUObject(this, &AFpsWaveCharacter::AttackFinished);
		Cont->OnReloadDelegate.AddUObject(this, &AFpsWaveCharacter::Reload);
	}

	if (EquippedWeapon)
	{
		CurrentDelegateHandle = EquippedWeapon->OnAttackDelegate.AddUObject(this, &AFpsWaveCharacter::PlayAttackMontage);
	}
}

// Called when the game starts or when spawned
void AFpsWaveCharacter::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetTimerManager().SetTimer(DelegateTimer, this, &AFpsWaveCharacter::BindDelegate, 0.2f);
	
	GetMesh()->HideBoneByName(FName("weapon_r"), PBO_None);
	GetMesh()->HideBoneByName(FName("weapon_l"), PBO_None);
	
	WeaponInventory.AttachedRifle = GetWorld()->SpawnActor<ARifle>(WeaponInventory.GetDefaultWeapon().GetDefaultRifle());
	WeaponInventory.AttachedRifle->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponInventory.AttachedShotgun = GetWorld()->SpawnActor<AShotgun>(WeaponInventory.GetDefaultWeapon().GetDefaultShotgun());
	WeaponInventory.AttachedShotgun->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponInventory.AttachedKatana = GetWorld()->SpawnActor<AKatana>(WeaponInventory.GetDefaultWeapon().GetDefaultKatana());
	WeaponInventory.AttachedKatana->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponInventory.AttachedHammer = GetWorld()->SpawnActor<AWarHammer>(WeaponInventory.GetDefaultWeapon().GetDefaultHammer());
	WeaponInventory.AttachedHammer->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EquippedWeapon = WeaponInventory.AttachedRifle;
	EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandGunSocket"));

	WeaponEquipType = EPlayerWeaponType::EPW_Rifle;

	BindDelegate();
}

void AFpsWaveCharacter::HideHeadBone()
{
	GetMesh()->HideBoneByName(FName("neck_01"), PBO_None);
}

void AFpsWaveCharacter::OnChangePointOfViewType()
{
	if (!FpsWaveController) return;

	EPointOfViewType PointOfViewType = FpsWaveController->GetPointOfViewType();
	GetWorld()->GetTimerManager().ClearTimer(ViewTimerHandle);

	if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ViewTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]
			{
				GetMesh()->HideBoneByName(FName("neck_01"), PBO_None);
			}),
			0.1f,
			false
			);
	}
	else
	{
		GetMesh()->UnHideBoneByName(FName("neck_01"));
	}
}

void AFpsWaveCharacter::OnFreeCameraStarted()
{
	TpsSpringArm->bUsePawnControlRotation = false;
	FpsSpringArm->bUsePawnControlRotation = false;
	TpsCrouchSpringArm->bUsePawnControlRotation = false;
	FpsCrouchSpringArm->bUsePawnControlRotation = false;
}

void AFpsWaveCharacter::OnFreeCameraCompleted()
{
	TpsSpringArm->bUsePawnControlRotation = true;
	FpsSpringArm->bUsePawnControlRotation = true;
	TpsCrouchSpringArm->bUsePawnControlRotation = true;
	FpsCrouchSpringArm->bUsePawnControlRotation = true;
}

// Called every frame
void AFpsWaveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFpsWaveCharacter::ChangeCapsuleSizeCrouch(EMoveState MoveState)
{
	auto Capsule = GetCapsuleComponent();
	
	if (MoveState == EMoveState::EMS_Crouch)
	{
		Capsule->SetCapsuleHalfHeight(70.f);
		Capsule->SetCapsuleRadius(40.f);
		GetMesh()->SetRelativeLocation(FVector(-25.f, 0.f, -65.f));
	}
	else
	{
		Capsule->SetCapsuleHalfHeight(90.f);
		Capsule->SetCapsuleRadius(25.f);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	}
	
}

void AFpsWaveCharacter::Interact()
{
	switch (OverlapDetectedType)
	{
	case EOverlapDetected::EOD_None:
		break;
	case EOverlapDetected::EOD_Weapon:

		if (!DetectedWeapon) return;

		EquippedWeapon->AttackFinished();
		
		//바닥에 떨어진 무기로 교체시 장착중인 무기를 바닥에 떨어져 있던 무기로 교체
		//set collision enabled 해제
		if (DetectedWeapon.IsA(AGun::StaticClass()))
		{
			AGun* Gun = Cast<AGun>(DetectedWeapon);
			Gun->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandGunSocket"));
			
			if (Gun->ActorHasTag(FName("Rifle")))
			{
				WeaponInventory.AttachedRifle = Cast<ARifle>(Gun);
				WeaponEquipType = EPlayerWeaponType::EPW_Rifle;
			}
			else if (Gun->ActorHasTag(FName("Shotgun")))
			{
				WeaponInventory.AttachedShotgun = Cast<AShotgun>(Gun);
				WeaponEquipType = EPlayerWeaponType::EPW_Shotgun;
			}

			EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedWeapon->SetActorHiddenInGame(true);
			EquippedWeapon = Gun;
		}
		else if (DetectedWeapon.IsA(AMelee::StaticClass()))
		{
			AMelee* Melee = Cast<AMelee>(DetectedWeapon);
			Melee->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandMeleeSocket"));
			
			if (Melee->ActorHasTag(FName("Katana")))
			{
				WeaponInventory.AttachedKatana = Cast<AKatana>(Melee);
				WeaponEquipType = EPlayerWeaponType::EPW_Katana;
			}
			else if (Melee->ActorHasTag(FName("Hammer")))
			{
				WeaponInventory.AttachedHammer = Cast<AWarHammer>(Melee);
				WeaponEquipType = EPlayerWeaponType::EPW_WarHammer;
			}

			EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedWeapon->SetActorHiddenInGame(true);
			EquippedWeapon = Melee;
		}

		if (SwapWeaponMontage && RootMotionAttackMontage)
		{
			if (WeaponEquipType == EPlayerWeaponType::EPW_Rifle || WeaponEquipType == EPlayerWeaponType::EPW_Shotgun)
			{
				GetMesh()->GetAnimInstance()->Montage_Play(SwapWeaponMontage);
			}
			else
			{
				GetMesh()->GetAnimInstance()->Montage_Play(RootMotionAttackMontage);
			}
		}

		if (OnWeaponChangeDelegate.IsBound())
		{
			OnWeaponChangeDelegate.Broadcast(EquippedWeapon);
		}
		
		DetectedWeapon = nullptr;
		OverlapDetectedType = EOverlapDetected::EOD_None;
		break;
	}
}

void AFpsWaveCharacter::ChangeWeapon_Key(EKeyInput Key)
{
	// 기존 attack montage델리게이트 해제
	EquippedWeapon->AttackFinished();
	EquippedWeapon->OnAttackDelegate.Remove(CurrentDelegateHandle);
	// 새로 장착할 무기와 현재 무기가 같은지 미리 확인
	AFpsWaveWeapon* NewWeapon = nullptr;
	FName SocketName;
    
	// 키에 따른 새 무기 결정
	if (Key == EKeyInput::EKI_1 || Key == EKeyInput::EKI_2)
	{
		if (Key == EKeyInput::EKI_1)
		{
			NewWeapon = WeaponInventory.AttachedRifle;
			WeaponEquipType = EPlayerWeaponType::EPW_Rifle;
		}
		else if (Key == EKeyInput::EKI_2)
		{
			NewWeapon = WeaponInventory.AttachedShotgun;
			WeaponEquipType = EPlayerWeaponType::EPW_Shotgun;
		}
		
		SocketName = FName("rightHandGunSocket");
	}
	else if (Key == EKeyInput::EKI_3 || Key == EKeyInput::EKI_4)
	{
		if (Key == EKeyInput::EKI_3)
		{
			NewWeapon = WeaponInventory.AttachedKatana;
			WeaponEquipType = EPlayerWeaponType::EPW_Katana;
		}
		else if (Key == EKeyInput::EKI_4)
		{
			NewWeapon = WeaponInventory.AttachedHammer;
			WeaponEquipType = EPlayerWeaponType::EPW_WarHammer;
		}
		
		SocketName = FName("rightHandMeleeSocket");
	}
    
	// 이미 같은 무기가 장착되어 있으면 아무것도 하지 않음
	if (EquippedWeapon == NewWeapon)
	{
		return;
	}
    
	// 유효한 새 무기가 없으면 함수 종료
	if (!NewWeapon || !NewWeapon->IsValidLowLevel())
	{
		return;
	}
    
	// 애니메이션 재생 (무기 교체 전)
	if (SwapWeaponMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SwapWeaponMontage);
	}
    
	// 기존 무기 해제
	if (EquippedWeapon && EquippedWeapon->IsValidLowLevel())
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon->SetActorHiddenInGame(true);
	}
    
	// 새 무기 장착
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	EquippedWeapon = NewWeapon;
	EquippedWeapon->SetActorHiddenInGame(false);
	EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, SocketName);
	// 새 attack montage델리게이트 바인딩
	CurrentDelegateHandle = EquippedWeapon->OnAttackDelegate.AddUObject(this, &AFpsWaveCharacter::PlayAttackMontage);

	if (OnWeaponChangeDelegate.IsBound())
	{
		OnWeaponChangeDelegate.Broadcast(EquippedWeapon);
	}
}

void AFpsWaveCharacter::ChangeWeapon_MouseWheel(int input)
{
	WeaponIndex -= input;

	if (WeaponIndex == 0)
	{
		WeaponIndex = 4;
	}

	if (WeaponIndex == 5)
	{
		WeaponIndex = 1;
	}

	EquippedWeapon->AttackFinished();
	// 기존 attack montage델리게이트 해제
	EquippedWeapon->OnAttackDelegate.Remove(CurrentDelegateHandle);

	if (EquippedWeapon && EquippedWeapon->IsValidLowLevel())
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon->SetActorHiddenInGame(true);
	}
	
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	
	if (WeaponIndex == 1 || WeaponIndex == 2)
	{
		if (WeaponIndex == 1)
		{
			EquippedWeapon = WeaponInventory.AttachedRifle;
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandGunSocket"));
			WeaponEquipType = EPlayerWeaponType::EPW_Rifle;
		}
		else if (WeaponIndex == 2)
		{
			EquippedWeapon = WeaponInventory.AttachedShotgun;
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandGunSocket"));
			WeaponEquipType = EPlayerWeaponType::EPW_Shotgun;
		}
	}
	else if (WeaponIndex == 3 || WeaponIndex == 4)
	{
		if (WeaponIndex == 3)
		{
			EquippedWeapon = WeaponInventory.AttachedKatana;
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandMeleeSocket"));
			WeaponEquipType = EPlayerWeaponType::EPW_Katana;
		}
		else if (WeaponIndex == 4)
		{
			EquippedWeapon = WeaponInventory.AttachedHammer;
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandMeleeSocket"));
			WeaponEquipType = EPlayerWeaponType::EPW_WarHammer;
		}
	}
	
	if (SwapWeaponMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SwapWeaponMontage);
	}
	EquippedWeapon->SetActorHiddenInGame(false);
	// 새 attack montage델리게이트 바인딩
	CurrentDelegateHandle = EquippedWeapon->OnAttackDelegate.AddUObject(this, &AFpsWaveCharacter::PlayAttackMontage);

	if (OnWeaponChangeDelegate.IsBound())
	{
		OnWeaponChangeDelegate.Broadcast(EquippedWeapon);
	}
}

void AFpsWaveCharacter::PlayAttackMontage()
{
	if (AttackMontage && RootMotionAttackMontage)
	{
		switch (WeaponEquipType)
		{
			//todo
		case EPlayerWeaponType::EPW_Rifle:
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Rifle"), AttackMontage);
			break;
		case EPlayerWeaponType::EPW_Shotgun:
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Shotgun"), AttackMontage);
			break;
		default:
			GetMesh()->GetAnimInstance()->Montage_Play(RootMotionAttackMontage);
			FName ComboName(FString("Combo" + FString::FromInt(EquippedWeapon->GetCurrentComboCount())));
			GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, ComboName.ToString());
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(ComboName, RootMotionAttackMontage);
			break;
		}
	}
}

void AFpsWaveCharacter::Attack()
{
	if (EquippedWeapon)
	{
		if (WeaponEquipType == EPlayerWeaponType::EPW_Katana || WeaponEquipType == EPlayerWeaponType::EPW_WarHammer)
		{
			if (bIsAttacking == true)
			{
				if (bCanCombo == true)
				{
					bQueuedCombo = true;
				}
				else
				{
					return;
				}
			}
			else
			{
				EquippedWeapon->AttackFinished();
				EquippedWeapon->Attack();
			}
		}
		else
		{
			EquippedWeapon->Attack();
		}
		
		PlayerAttackState = EAttackState::EAS_Attack;
	}
}

void AFpsWaveCharacter::AttackFinished()
{
	if (EquippedWeapon)
	{
		if (WeaponEquipType == EPlayerWeaponType::EPW_Katana || WeaponEquipType == EPlayerWeaponType::EPW_WarHammer)
		{
			/*if (bCanCombo == false)
			{
				EquippedWeapon->AttackFinished();
			}*/
		}
		else
		{
			EquippedWeapon->AttackFinished();
		}

		PlayerAttackState = EAttackState::EAS_None;
	}
}

void AFpsWaveCharacter::Reload()
{
	if (PlayerAttackState == EAttackState::EAS_Reload)
	{
		return;
	}
	
	if (WeaponEquipType == EPlayerWeaponType::EPW_Rifle || WeaponEquipType == EPlayerWeaponType::EPW_Shotgun)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reload"));
		PlayerAttackState = EAttackState::EAS_Reload;

		auto AnimInstance = GetMesh()->GetAnimInstance();
		
		AnimInstance->Montage_Play(ReloadMontage);

		switch (WeaponEquipType)
		{
		case EPlayerWeaponType::EPW_Rifle:
			AnimInstance->Montage_JumpToSection("Rifle");
			break;
		case EPlayerWeaponType::EPW_Shotgun:
			AnimInstance->Montage_JumpToSection("Shotgun");
			break;
		default:
			break;
		}
	}
}

//ABP에서 ReloadEndNotify호출시 실행
void AFpsWaveCharacter::ReloadEnd()
{
	PlayerAttackState = EAttackState::EAS_Attack;

	if (auto Gun = Cast<AGun>(EquippedWeapon))
	{
		Gun->ResetCurrentBulletCountToMax();
	}
}

#pragma region Getter/Setters
TObjectPtr<UChildActorComponent> AFpsWaveCharacter::GetFpsCameraChildActor()
{
	return FpsCameraChildActor;
}

TObjectPtr<UChildActorComponent> AFpsWaveCharacter::GetTpsCrouchCameraChildActor()
{
	return TpsCrouchCameraChildActor;
}

TObjectPtr<UChildActorComponent> AFpsWaveCharacter::GetTpsCameraChildActor()
{
	return TpsCameraChildActor;
}

TObjectPtr<USpringArmComponent> AFpsWaveCharacter::GetFpsCrouchSpringArm()
{
	return FpsCrouchSpringArm;
}

TObjectPtr<USpringArmComponent> AFpsWaveCharacter::GetFpsSpringArm()
{
	return FpsSpringArm;
}

TObjectPtr<USpringArmComponent> AFpsWaveCharacter::GetTpsCrouchSpringArm()
{
	return TpsCrouchSpringArm;
}

TObjectPtr<USpringArmComponent> AFpsWaveCharacter::GetTpsSpringArm()
{
	return TpsSpringArm;
}

TObjectPtr<UChildActorComponent> AFpsWaveCharacter::GetFpsCrouchCameraChildActor()
{
	return FpsCrouchCameraChildActor;
}

TObjectPtr<UChildActorComponent> AFpsWaveCharacter::GetTpsZoomInCameraChildActor()
{
	return TpsZoomInCameraChildActor;
}

TObjectPtr<AFpsWaveWeapon> AFpsWaveCharacter::GetDetectedWeapon()
{
	return DetectedWeapon;
}

void AFpsWaveCharacter::SetDetectedWeapon(AFpsWaveWeapon* Weapon)
{
	this->DetectedWeapon = Weapon;
}

EOverlapDetected AFpsWaveCharacter::GetOverlapDetectedType() const
{
	return OverlapDetectedType;
}

void AFpsWaveCharacter::SetOverlapDetectedType(EOverlapDetected DetectedType)
{
	this->OverlapDetectedType = DetectedType;
}

EPlayerWeaponType AFpsWaveCharacter::GetPlayerWeaponType() const
{
	return WeaponEquipType;
}

EAttackState AFpsWaveCharacter::GetPlayerAttackState() const
{
	return PlayerAttackState;
}

TObjectPtr<AFpsWaveWeapon> AFpsWaveCharacter::GetEquippedWeapon()
{
	return EquippedWeapon;
}

void AFpsWaveCharacter::SetIsAttacking(bool IsAttacking)
{
	this->bIsAttacking = IsAttacking;
}

void AFpsWaveCharacter::SetCanCombo(bool CanCombo)
{
	this->bCanCombo = CanCombo;
}

bool AFpsWaveCharacter::GetQueuedCombo()
{
	return bQueuedCombo;
}

void AFpsWaveCharacter::SetQueuedCombo(bool QueuedCombo)
{
	this->bQueuedCombo = QueuedCombo;
}

#pragma endregion
