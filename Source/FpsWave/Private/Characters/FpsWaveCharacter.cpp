// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsWave/Public/Characters/FpsWaveCharacter.h"

#include "Camera/CameraComponent.h"
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

		GetMesh()->HideBoneByName(FName("weapon_r"), PBO_None);
		GetMesh()->HideBoneByName(FName("weapon_l"), PBO_None);
	}

	//todo , 소켓에 부착되게하기
	WeaponInventory.AttachedRifle = GetWorld()->SpawnActor<AGun>(WeaponInventory.GetDefaultWeapon().GetDefaultRifle(), FVector::Zero(), FRotator::ZeroRotator);
	WeaponInventory.AttachedShotgun = GetWorld()->SpawnActor<AGun>(WeaponInventory.GetDefaultWeapon().GetDefaultShotgun(), FVector::Zero(), FRotator::ZeroRotator);
	WeaponInventory.AttachedKatana = GetWorld()->SpawnActor<AMelee>(WeaponInventory.GetDefaultWeapon().GetDefaultKatana(), FVector::Zero(), FRotator::ZeroRotator);
	WeaponInventory.AttachedHammer = GetWorld()->SpawnActor<AMelee>(WeaponInventory.GetDefaultWeapon().GetDefaultHammer(), FVector::Zero(), FRotator::ZeroRotator);

	EquippedWeapon = WeaponInventory.AttachedRifle;
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	EquippedWeapon->AttachToComponent(GetMesh(), Rules, FName("rightHandGunSocket"));
	EquippedWeapon->GetItemMesh()->SetActive(false);
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
		//todo 교체시 아이템교체
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
		}
		
		DetectedWeapon = nullptr;
		break;
	}
}



