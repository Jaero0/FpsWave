// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FpsWaveCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FpsWaveCharacter.h"
#include "DataAssets/InputDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"

void AFpsWaveCharacterController::ChangeRunToggle()
{
	bIsRunToggleMode = !bIsRunToggleMode;
}

void AFpsWaveCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (MappingContext)
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
			Player = Cast<AFpsWaveCharacter>(GetOwner());
		}
	}
}

void AFpsWaveCharacterController::Move(const FInputActionValue &InputActionValue)
{
	FVector2D Vector2D = InputActionValue.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(Vector2D.ToString()));

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	GetPawn()->AddMovementInput(ForwardDirection, Vector2D.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetPawn()->AddMovementInput(RightDirection, Vector2D.X);
}

void AFpsWaveCharacterController::RunInputStarted(const FInputActionValue& InputActionValue)
{
	// 홀드 모드
	if (bIsRunToggleMode == false)
	{
		bIsRunning = true;
		UpdateMoveSpeed();
		return;
	}

	// 토글 모드
	bIsRunning = !bIsRunning;
	UpdateMoveSpeed();
}

void AFpsWaveCharacterController::RunInputCompleted(const FInputActionValue& InputActionValue)
{
	// 홀드 모드에서만 처리
	if (!bIsRunToggleMode)
	{
		bIsRunning = false;
		UpdateMoveSpeed();
	}
	// 토글 모드는 Completed 시점에 아무것도 안 함
}

void AFpsWaveCharacterController::UpdateMoveSpeed()
{
	Player->GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunningSpeed : WalkSpeed;
}

void AFpsWaveCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (InputDataAsset)
		{
			EnhancedInputComponent->BindAction(InputDataAsset->MoveAction, ETriggerEvent::Triggered, this, &AFpsWaveCharacterController::Move);
			EnhancedInputComponent->BindAction(InputDataAsset->RunAction, ETriggerEvent::Started, this, &AFpsWaveCharacterController::RunInputStarted);
			EnhancedInputComponent->BindAction(InputDataAsset->RunAction, ETriggerEvent::Completed, this, &AFpsWaveCharacterController::RunInputCompleted);
		}
	}
}


