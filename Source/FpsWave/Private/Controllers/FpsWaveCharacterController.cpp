// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FpsWaveCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FpsWaveCharacter.h"
#include "Components/CheckBox.h"
#include "DataAssets/InputDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/Toggles.h"

void AFpsWaveCharacterController::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AFpsWaveCharacter>(GetPawn());

	if (MappingContext)
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AFpsWaveCharacterController::Move(const FInputActionValue &InputActionValue)
{
	FVector2D Vector2D = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	GetPawn()->AddMovementInput(ForwardDirection, Vector2D.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetPawn()->AddMovementInput(RightDirection, Vector2D.X);
}

void AFpsWaveCharacterController::UpdateMoveSpeed()
{
	AFpsWaveCharacter* CurrentPlayer = Player;
	if (!CurrentPlayer)
	{
		CurrentPlayer = Cast<AFpsWaveCharacter>(GetPawn());

		if (!CurrentPlayer)
		{
			UE_LOG(LogTemp, Error, TEXT("Controller에서 Player를 찾을 수 없음"));
			return;
		}

		Player = CurrentPlayer;
	}

	if (UCharacterMovementComponent* Movement = Player->GetCharacterMovement())
	{
		switch (CharacterMoveState)
		{
		case EMoveState::EMS_Run:
			Movement->MaxWalkSpeed = RunningSpeed;
			break;
		case EMoveState::EMS_Walk:
			Movement->MaxWalkSpeed = WalkSpeed;
			break;
		case EMoveState::EMS_Crouch:
			//todo
			Movement->MaxWalkSpeed = CrouchSpeed;
			break;
		}
	}
}

void AFpsWaveCharacterController::OnChangeRunToggle(bool bIsChecked)
{
	RunToggleMode = bIsChecked ? EToggleMode::ETM_Toggle : EToggleMode::ETM_None;
}

void AFpsWaveCharacterController::OnChangeCrouchToggle(bool bIsChecked)
{
	CrouchToggleMode = bIsChecked ? EToggleMode::ETM_Toggle : EToggleMode::ETM_None;
}

void AFpsWaveCharacterController::RunInputStarted(const FInputActionValue& InputActionValue)
{
	if (RunToggleMode == EToggleMode::ETM_None) // 홀드 모드
	{
		CharacterMoveState = EMoveState::EMS_Run;
		UpdateMoveSpeed();
		return;
	}

	// 토글 모드
	CharacterMoveState = (CharacterMoveState == EMoveState::EMS_Run)
		? EMoveState::EMS_Walk
		: EMoveState::EMS_Run;

	UpdateMoveSpeed();
}


void AFpsWaveCharacterController::CrouchInputStarted(const FInputActionValue& InputActionValue)
{
	if (CrouchToggleMode == EToggleMode::ETM_None) // 홀드 모드
	{
		CharacterMoveState = EMoveState::EMS_Crouch;
		UpdateMoveSpeed();
		return;
	}

	// 토글 모드
	CharacterMoveState = (CharacterMoveState == EMoveState::EMS_Crouch)
		? EMoveState::EMS_Walk
		: EMoveState::EMS_Crouch;

	UpdateMoveSpeed();
}

void AFpsWaveCharacterController::RunInputCompleted(const FInputActionValue& InputActionValue)
{
	if (RunToggleMode == EToggleMode::ETM_None) // 홀드 모드에서만 처리
	{
		if (CharacterMoveState == EMoveState::EMS_Run)
		{
			CharacterMoveState = EMoveState::EMS_Walk;
			UpdateMoveSpeed();
		}
	}
}

void AFpsWaveCharacterController::CrouchInputCompleted(const FInputActionValue& InputActionValue)
{
	if (CrouchToggleMode == EToggleMode::ETM_None) // 홀드 모드에서만 처리
	{
		if (CharacterMoveState == EMoveState::EMS_Crouch)
		{
			CharacterMoveState = EMoveState::EMS_Walk;
			UpdateMoveSpeed();
		}
	}
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
			EnhancedInputComponent->BindAction(InputDataAsset->CrouchAction, ETriggerEvent::Started, this, &AFpsWaveCharacterController::CrouchInputStarted);
			EnhancedInputComponent->BindAction(InputDataAsset->CrouchAction, ETriggerEvent::Completed, this, &AFpsWaveCharacterController::CrouchInputCompleted);
		}
	}
}


