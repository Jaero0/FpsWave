// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsWave/Public/Characters/FpsWaveCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AFpsWaveCharacter::AFpsWaveCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	TpsSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TpsSpringArm"));
	TpsSpringArm->SetupAttachment(GetRootComponent());
	TpsSpringArm->TargetArmLength = DefaultTpsSpringArmLength;
	TpsCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("TpsCamera"));
	TpsCameraChildActor->SetupAttachment(TpsSpringArm);

	FpsSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FpsSpringArm"));
	FpsSpringArm->SetupAttachment(GetRootComponent());
	FpsSpringArm->TargetArmLength = DefaultFpsSpringArmLength;
	FpsCameraChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("FpsCamera"));
	FpsCameraChildActor->SetupAttachment(FpsSpringArm);

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
	}
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
		GetWorld()->GetTimerManager().SetTimer(ViewTimerHandle, this, &AFpsWaveCharacter::HideHeadBone, 0.1f, false);
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
}

void AFpsWaveCharacter::OnFreeCameraCompleted()
{
	TpsSpringArm->bUsePawnControlRotation = true;
	FpsSpringArm->bUsePawnControlRotation = true;
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

