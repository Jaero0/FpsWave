// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FpsWaveCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Characters/FpsWaveCharacter.h"
#include "Components/CheckBox.h"
#include "DataAssets/InputDataAsset.h"
#include "DSP/LFO.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	
	if (OnTpsFpsTypeChangedDelegate.IsBound())
	{
		OnTpsFpsTypeChangedDelegate.Execute();
	}
}

void AFpsWaveCharacterController::Move(const FInputActionValue &InputActionValue)
{
	FVector2D Vector2D = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetPawn()->AddMovementInput(ForwardDirection, Vector2D.Y);
	GetPawn()->AddMovementInput(RightDirection, Vector2D.X);
}

#pragma region 시점 제어

void AFpsWaveCharacterController::TpsFpsConversion()
{
	if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
	{
		PointOfViewType = EPointOfViewType::EPT_ThirdPersonView;
	}
	else
	{
		PointOfViewType = EPointOfViewType::EPT_FirstPersonView;
	}

	OnTpsFpsTypeChangedDelegate.Execute();
}

void AFpsWaveCharacterController::LookFreeCameraStarted()
{
	CurrentFreeCamYaw = 0.f;
	CurrentFreeCamPitch = 0.f;
    
	if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
	{
		FreeCameraStartedRotation = Player->GetFpsCamera()->GetComponentRotation();
	}
	else
	{
		FreeCameraStartedRotation = Player->GetTpsSpringArm()->GetRelativeRotation();
	}

	bIsFreeCamStarted = true;
	OnFreeCameraStartedDelegate.Execute();
}

void AFpsWaveCharacterController::Look(const FInputActionValue& InputActionValue)
{
	FVector2D LookAxis = InputActionValue.Get<FVector2D>();
	
	if (bIsFreeCamStarted == true)
	{
		if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
		{
			CurrentFreeCamPitch += LookAxis.Y;
			CurrentFreeCamPitch = FMath::Clamp(CurrentFreeCamPitch, -60.f, 70.f);

			CurrentFreeCamYaw += LookAxis.X;
			CurrentFreeCamYaw = FMath::Clamp(CurrentFreeCamYaw, -70.f, 70.f);

			// 월드 회전값으로 직접 설정
			FRotator NewWorldRot = FreeCameraStartedRotation;
			NewWorldRot.Pitch += CurrentFreeCamPitch;
			NewWorldRot.Yaw += CurrentFreeCamYaw;
			Player->GetFpsCamera()->SetWorldRotation(NewWorldRot); 
		}
		else
		{
			CurrentFreeCamPitch += LookAxis.Y;
			CurrentFreeCamPitch = FMath::Clamp(CurrentFreeCamPitch, -60.f, 70.f);

			CurrentFreeCamYaw += LookAxis.X;

			// 시작 회전값에 누적된 값을 더해서 새로운 회전값 계산
			FRotator NewRot = FreeCameraStartedRotation;
			NewRot.Pitch += CurrentFreeCamPitch;
			NewRot.Yaw += CurrentFreeCamYaw;
			Player->GetTpsSpringArm()->SetRelativeRotation(NewRot);
		}
	}
	else
	{
		AddYawInput(LookAxis.X);

		if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
		{
			AddPitchInput(LookAxis.Y * -1);

			FRotator ControlRot = GetControlRotation();
			ControlRot.Pitch = FMath::ClampAngle(ControlRot.Pitch, -60.f, 70.f);
			SetControlRotation(ControlRot);
		}
		else
		{
			FRotator NewRot = Player->GetTpsSpringArm()->GetRelativeRotation();
			NewRot.Pitch = FMath::Clamp(NewRot.Pitch + LookAxis.Y, -60.f, 70.f);
			Player->GetTpsSpringArm()->SetRelativeRotation(NewRot);
		}
	}
}

void AFpsWaveCharacterController::LookFreeCameraCompleted()
{
	if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
	{
		Player->GetFpsCamera()->SetRelativeRotation(FreeCameraStartedRotation);
	}
	else
	{
		Player->GetTpsSpringArm()->SetRelativeRotation(FreeCameraStartedRotation);
	}

	CurrentFreeCamPitch = 0.f;
	CurrentFreeCamYaw = 0.f;
	bIsFreeCamStarted = false;

	OnFreeCameraCompletedDelegate.Execute();
}

#pragma endregion

#pragma region 달리기 / 웅크리기 움직임 속도 조절 메서드
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


#pragma endregion

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
			EnhancedInputComponent->BindAction(InputDataAsset->FpsTpsConversionAction, ETriggerEvent::Started, this, &AFpsWaveCharacterController::TpsFpsConversion);
			EnhancedInputComponent->BindAction(InputDataAsset->LookAction, ETriggerEvent::Triggered, this, &AFpsWaveCharacterController::Look);
			EnhancedInputComponent->BindAction(InputDataAsset->FreeCameraAction, ETriggerEvent::Started, this, &AFpsWaveCharacterController::LookFreeCameraStarted);
			//EnhancedInputComponent->BindAction(InputDataAsset->FreeCameraAction, ETriggerEvent::Triggered, this, &AFpsWaveCharacterController::LookFreeCameraTriggered);
			EnhancedInputComponent->BindAction(InputDataAsset->FreeCameraAction, ETriggerEvent::Completed, this, &AFpsWaveCharacterController::LookFreeCameraCompleted);
		}
	}
}

