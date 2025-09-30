// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsWave/Public/Characters/FpsWaveCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon/FpsWaveWeapon.h"
#include "Weapon/Gun/Gun.h"
#include "Weapon/Melee/Melee.h"


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

// Called when the game starts or when spawned
void AFpsWaveCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (auto Cont = Cast<AFpsWaveCharacterController>(GetWorld()->GetFirstPlayerController()))
	{
		FpsWaveController = Cont;
		Cont->OnTpsFpsTypeChangedDelegate.BindUObject(this, &AFpsWaveCharacter::OnChangePointOfViewType);
		Cont->OnFreeCameraStartedDelegate.BindUObject(this, &AFpsWaveCharacter::OnFreeCameraStarted);
		Cont->OnFreeCameraCompletedDelegate.BindUObject(this, &AFpsWaveCharacter::OnFreeCameraCompleted);
		Cont->OnInteractionDelegate.BindUObject(this, &AFpsWaveCharacter::Interact);
		Cont->OnWeaponChange_Key_Delegate.BindUObject(this, &AFpsWaveCharacter::ChangeWeapon_Key);
		Cont->OnWeaponChange_MouseWheel_Delegate.BindUObject(this, &AFpsWaveCharacter::ChangeWeapon_MouseWheel);

		GetMesh()->HideBoneByName(FName("weapon_r"), PBO_None);
		GetMesh()->HideBoneByName(FName("weapon_l"), PBO_None);
	}

	//todo, 시작무기 = Rifle, 장착 시 Collision overlap 해제
	WeaponInventory.AttachedRifle = GetWorld()->SpawnActor<AGun>(WeaponInventory.GetDefaultWeapon().GetDefaultRifle());
	WeaponInventory.AttachedRifle->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponInventory.AttachedShotgun = GetWorld()->SpawnActor<AGun>(WeaponInventory.GetDefaultWeapon().GetDefaultShotgun());
	WeaponInventory.AttachedShotgun->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponInventory.AttachedKatana = GetWorld()->SpawnActor<AMelee>(WeaponInventory.GetDefaultWeapon().GetDefaultKatana());
	WeaponInventory.AttachedKatana->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponInventory.AttachedHammer = GetWorld()->SpawnActor<AMelee>(WeaponInventory.GetDefaultWeapon().GetDefaultHammer());
	WeaponInventory.AttachedHammer->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EquippedWeapon = WeaponInventory.AttachedRifle;
	EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandGunSocket"));

	EquipType = EPlayerEquipType::EPE_Gun;
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
			FTimerDelegate::CreateWeakLambda(this, [this]()
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

EPlayerEquipType AFpsWaveCharacter::GetPlayerEquipType() const
{
	return EquipType;
}

// Called every frame
void AFpsWaveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFpsWaveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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
		//감지된 무기 장착
		//todo 교체시 아이템교체, set collision enabled 해제
		if (DetectedWeapon.IsA(AGun::StaticClass()))
		{
			AGun* Gun = Cast<AGun>(DetectedWeapon);
			Gun->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandGunSocket"));
			
			if (Gun->ActorHasTag(FName("Rifle")))
			{
				WeaponInventory.AttachedRifle = Gun;
			}
			else if (Gun->ActorHasTag(FName("Shotgun")))
			{
				WeaponInventory.AttachedShotgun = Gun;
			}

			EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedWeapon->SetActorHiddenInGame(true);
			EquippedWeapon = Gun;
			EquipType = EPlayerEquipType::EPE_Gun;
		}
		else if (DetectedWeapon.IsA(AMelee::StaticClass()))
		{
			AMelee* Melee = Cast<AMelee>(DetectedWeapon);
			Melee->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandMeleeSocket"));
			
			if (Melee->ActorHasTag(FName("Katana")))
			{
				WeaponInventory.AttachedKatana = Melee;
			}
			else if (Melee->ActorHasTag(FName("Hammer")))
			{
				WeaponInventory.AttachedHammer = Melee;
			}

			EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedWeapon->SetActorHiddenInGame(true);
			EquippedWeapon = Melee;
			EquipType = EPlayerEquipType::EPE_Melee;
		}

		if (SwapWeaponMontage)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(SwapWeaponMontage);
		}
		
		DetectedWeapon = nullptr;
		OverlapDetectedType = EOverlapDetected::EOD_None;
		break;
	}

	
}

void AFpsWaveCharacter::ChangeWeapon_Key(int key)
{
	// 새로 장착할 무기와 현재 무기가 같은지 미리 확인
	AFpsWaveWeapon* NewWeapon = nullptr;
	FName SocketName;
	EPlayerEquipType NewEquipType;
    
	// 키에 따른 새 무기 결정
	if (key == 1 || key == 2)
	{
		if (key == 1)
		{
			NewWeapon = WeaponInventory.AttachedRifle;
		}
		else if (key == 2)
		{
			NewWeapon = WeaponInventory.AttachedShotgun;
		}
		SocketName = FName("rightHandGunSocket");
		NewEquipType = EPlayerEquipType::EPE_Gun;
	}
	else if (key == 3 || key == 4)
	{
		if (key == 3)
		{
			NewWeapon = WeaponInventory.AttachedKatana;
		}
		else if (key == 4)
		{
			NewWeapon = WeaponInventory.AttachedHammer;
		}
		SocketName = FName("rightHandMeleeSocket");
		NewEquipType = EPlayerEquipType::EPE_Melee;
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
	EquipType = NewEquipType;
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

	if (EquippedWeapon && EquippedWeapon->IsValidLowLevel())
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon->SetActorHiddenInGame(true);
	}
	
	if (WeaponIndex == 1 || WeaponIndex == 2)
	{
		if (WeaponIndex == 1)
		{
			EquippedWeapon = WeaponInventory.AttachedRifle;
			FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandGunSocket"));
		}
		else if (WeaponIndex == 2)
		{
			EquippedWeapon = WeaponInventory.AttachedShotgun;
			FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandGunSocket"));
		}
		
		EquipType = EPlayerEquipType::EPE_Gun;
	}
	else if (WeaponIndex == 3 || WeaponIndex == 4)
	{
		if (WeaponIndex == 3)
		{
			EquippedWeapon = WeaponInventory.AttachedKatana;
			FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), Rules, FName("rightHandMeleeSocket"));
		}
		else if (WeaponIndex == 4)
		{
			EquippedWeapon = WeaponInventory.AttachedHammer;
			EquippedWeapon->GetItemMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("rightHandMeleeSocket"));
		}
		
		EquipType = EPlayerEquipType::EPE_Melee;
	}
	
	if (SwapWeaponMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SwapWeaponMontage);
	}
	EquippedWeapon->SetActorHiddenInGame(false);
}



