// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsWave/Public/Characters/FpsWaveCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AFpsWaveCharacter::AFpsWaveCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	TpsSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TpsSpringArm"));
	TpsSpringArm->SetupAttachment(GetRootComponent());
	TpsSpringArm->TargetArmLength = MaxTpsSpringArmLength;
	
	TpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamera"));
	TpsCamera->SetupAttachment(TpsSpringArm);
	
	FpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCamera"));
	FpsCamera->SetActive(false);
	FpsCamera->SetupAttachment(GetMesh(), TEXT("headSocket"));

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = UE::Math::TRotator(0.0, 460.0, 0.0);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void AFpsWaveCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (auto Cont = Cast<AFpsWaveCharacterController>(GetLocalViewingPlayerController()))
	{
		FpsWaveController = Cont;
		Cont->OnTpsFpsTypeChangedDelegate.BindUObject(this, &AFpsWaveCharacter::OnChangePointOfViewType);
		Cont->OnFreeCameraStartedDelegate.BindUObject(this, &AFpsWaveCharacter::OnFreeCameraStarted);
		Cont->OnFreeCameraCompletedDelegate.BindUObject(this, &AFpsWaveCharacter::OnFreeCameraCompleted);
	}
}

void AFpsWaveCharacter::OnChangePointOfViewType()
{
	bIsCameraConversionTriggered = true;
	
	if (!FpsWaveController) return;

	EPointOfViewType ViewType = FpsWaveController->GetPointOfViewType();

	if (ViewType == EPointOfViewType::EPT_FirstPersonView)
	{
		CurrentTargetArmLength = MinTpsSpringArmLength;
		
		if (FpsWaveController)
		{
			FpsWaveController->SetControlRotation(FRotator(-15.f, GetActorRotation().Yaw, 0.f));
		}
		FpsCamera->SetRelativeRotation(DefaultFpsCameraRot);

		FpsCamera->SetActive(true);
		TpsCamera->SetActive(false);
		TpsSpringArm->SetActive(false);
	}
	else if (ViewType == EPointOfViewType::EPT_ThirdPersonView)
	{
		CurrentTargetArmLength = MaxTpsSpringArmLength;

		TpsSpringArm->SetRelativeRotation(DefaultTpsSpringArmRot);
		
		FpsCamera->SetActive(false);
		TpsCamera->SetActive(true);
		TpsSpringArm->SetActive(true);
	}
}

void AFpsWaveCharacter::OnFreeCameraStarted()
{
	if (FpsWaveController && FpsWaveController->GetPointOfViewType() == EPointOfViewType::EPT_FirstPersonView)
	{
		FpsCamera->SetupAttachment(GetRootComponent());
		FpsCamera->bUsePawnControlRotation = false;
	}
}

void AFpsWaveCharacter::OnFreeCameraCompleted()
{
	if (FpsWaveController && FpsWaveController->GetPointOfViewType() == EPointOfViewType::EPT_FirstPersonView)
	{
		FpsCamera->SetupAttachment(GetMesh(), TEXT("headSocket"));
		FpsCamera->bUsePawnControlRotation = true;
	}
}

// Called every frame
void AFpsWaveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCameraConversionTriggered == true)
	{
		float Current = TpsSpringArm->TargetArmLength;
		float Interp = FMath::FInterpTo(Current, CurrentTargetArmLength, DeltaTime, CameraConversionInterpSpeed);
		TpsSpringArm->TargetArmLength = Interp;
		
		if (FMath::IsNearlyEqual(TpsSpringArm->TargetArmLength, CurrentTargetArmLength))
		{
			bIsCameraConversionTriggered = false;
		}
	}
}

// Called to bind functionality to input
void AFpsWaveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

