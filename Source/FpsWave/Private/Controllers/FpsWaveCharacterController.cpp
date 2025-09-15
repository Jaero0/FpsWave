// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FpsWaveCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FpsWaveCharacter.h"
#include "DataAssets/InputDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"

void AFpsWaveCharacterController::ChangeRunToggle()
{
	if (ToggleMode == EToggleMode::ETM_ToggleRun)
	{
		ToggleMode = EToggleMode::ETM_ToggleNone;
	}
	else
	{
		ToggleMode = EToggleMode::ETM_ToggleRun;
	}
}

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
	// 홀드모드
	if (ToggleMode == EToggleMode::ETM_ToggleNone)
	{
		CharacterMoveState = EMoveState::EMS_Run;
		UpdateMoveSpeed();
		return;
	}

	// 토글모드
	if (CharacterMoveState == EMoveState::EMS_Run)
	{
		CharacterMoveState = EMoveState::EMS_Walk;
	}
	else //run상태가 아니면 즉시 run으로 바꿔줌
	{
		CharacterMoveState = EMoveState::EMS_Run;
	}
	
	UpdateMoveSpeed();
}

void AFpsWaveCharacterController::RunInputCompleted(const FInputActionValue& InputActionValue)
{
	// 홀드모드에서만 처리
	if (ToggleMode == EToggleMode::ETM_ToggleNone)
	{
		CharacterMoveState = EMoveState::EMS_Walk;
		UpdateMoveSpeed();
	}
	// 토글모드는 Completed 시점에 아무것도 안 함
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
			break;
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
		}
	}
}


