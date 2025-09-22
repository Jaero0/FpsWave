// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/FpsWaveCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/FpsWaveCharacter.h"
#include "DataAssets/InputDataAsset.h"
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

	SetViewTarget(Player->GetTpsCameraChildActor()->GetChildActor());

	if (WidgetClass)
	{
		if (UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass))
		{
			UserWidget->AddToViewport();
		}
	}

	PlayerCameraManager->ViewPitchMin = -60.f;
	PlayerCameraManager->ViewPitchMax = 70.f;
}

void AFpsWaveCharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsReturningFromFreeCam)
	{
		InterpolateFreeCamToOriginCam(DeltaSeconds);
	}
}

bool AFpsWaveCharacterController::GetIsFreeCamStarted() const
{
	return bIsFreeCamStarted;
}

void AFpsWaveCharacterController::SetCharacterMoveStateUpdateSpeed(EMoveState MoveState)
{
	CharacterMoveState = MoveState;
	UpdateMoveSpeed();
}

void AFpsWaveCharacterController::Move(const FInputActionValue &InputActionValue)
{
	FVector2D Vector2D = InputActionValue.Get<FVector2D>();

	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(Vector2D.ToString()));

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
		SetViewTargetWithBlend(Player->GetTpsCameraChildActor()->GetChildActor(), 0.2f);
		
	}
	else
	{
		PointOfViewType = EPointOfViewType::EPT_FirstPersonView;
		SetViewTargetWithBlend(Player->GetFpsCameraChildActor()->GetChildActor(), 0.2f);
	}

	if (CharacterMoveState == EMoveState::EMS_ZoomInWalk)
	{
		SetCharacterMoveStateUpdateSpeed(EMoveState::EMS_Walk);
	}
	if (OnTpsFpsTypeChangedDelegate.IsBound())
	{
		OnTpsFpsTypeChangedDelegate.Execute();
	}
}

void AFpsWaveCharacterController::LookFreeCameraStarted()
{
	CurrentFreeCamYaw = 0.f;
	CurrentFreeCamPitch = 0.f;

	// 현재 컨트롤러 회전을 SpringArm의 RelativeRotation으로 설정
	FRotator CurrentControlRotation = GetControlRotation();
	FRotator ActorRotation = Player->GetActorRotation();
    
	// 액터 회전을 기준으로 한 상대적인 회전 계산
	FRotator RelativeRotation = CurrentControlRotation - ActorRotation;
    
	if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
	{
		Player->GetFpsSpringArm()->SetRelativeRotation(RelativeRotation);
		FreeCameraStartedRotation = Player->GetFpsSpringArm()->GetRelativeRotation();
	}
	else
	{
		Player->GetTpsSpringArm()->SetRelativeRotation(RelativeRotation);
		FreeCameraStartedRotation = Player->GetTpsSpringArm()->GetRelativeRotation();
	}

	bIsFreeCamStarted = true;
	OnFreeCameraStartedDelegate.ExecuteIfBound();
}
void AFpsWaveCharacterController::Look(const FInputActionValue& InputActionValue)
{
	FVector2D LookAxis = InputActionValue.Get<FVector2D>();
	
	if (bIsFreeCamStarted == true)
	{
		//상하
		CurrentFreeCamPitch += LookAxis.Y;
		float AbsolutePitch = FreeCameraStartedRotation.Pitch + CurrentFreeCamPitch;
		AbsolutePitch = FMath::Clamp(AbsolutePitch, -60.f, 70.f);
		CurrentFreeCamPitch = AbsolutePitch - FreeCameraStartedRotation.Pitch;

		//좌우
		CurrentFreeCamYaw += LookAxis.X;
		if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
		{
			CurrentFreeCamYaw = FMath::Clamp(CurrentFreeCamYaw, -70.f, 70.f);
		}

		FRotator NewRot = FreeCameraStartedRotation;
		NewRot.Pitch += CurrentFreeCamPitch;
		NewRot.Yaw += CurrentFreeCamYaw;

		if (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
		{
			Player->GetFpsSpringArm()->SetRelativeRotation(NewRot);
		}
		else
		{
			Player->GetTpsSpringArm()->SetRelativeRotation(NewRot);
		}
	}
	else
	{
		AddYawInput(LookAxis.X);
		AddPitchInput(LookAxis.Y * -1);
	}
}

void AFpsWaveCharacterController::LookFreeCameraCompleted()
{
	//Tick메서드에서 보간으로 카메라 원위치 시키는중

	CurrentFreeCamPitch = 0.f;
	CurrentFreeCamYaw = 0.f;
	bIsFreeCamStarted = false;
	bIsReturningFromFreeCam = true;
}

void AFpsWaveCharacterController::InterpolateFreeCamToOriginCam(float DeltaTime)
{
	USpringArmComponent* TargetSpringArm = (PointOfViewType == EPointOfViewType::EPT_FirstPersonView)
		   ? Player->GetFpsSpringArm()
		   : Player->GetTpsSpringArm();

	FRotator CurrentRot = TargetSpringArm->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpConstantTo(CurrentRot, FreeCameraStartedRotation, DeltaTime, CamReturningInterpSpeed);

	TargetSpringArm->SetRelativeRotation(NewRot);

	// 목표에 충분히 가까워지면 보간 종료
	if (NewRot.Equals(FreeCameraStartedRotation, 1.f))
	{
		TargetSpringArm->SetRelativeRotation(FreeCameraStartedRotation);
		bIsReturningFromFreeCam = false;

		OnFreeCameraCompletedDelegate.ExecuteIfBound();
	}
}

void AFpsWaveCharacterController::ZoomInStarted()
{
	if (PointOfViewType == EPointOfViewType::EPT_ThirdPersonView)
	{
		SetViewTargetWithBlend(Player->GetTpsZoomInCameraChildActor()->GetChildActor(), 0.1f);
		PointOfViewType = EPointOfViewType::EPT_ThirdPersonZoomInView;
		SetCharacterMoveStateUpdateSpeed(EMoveState::EMS_ZoomInWalk);
	}
}

void AFpsWaveCharacterController::ZoomInCompleted()
{
	if (PointOfViewType == EPointOfViewType::EPT_ThirdPersonZoomInView)
	{
		SetViewTargetWithBlend(Player->GetTpsCameraChildActor()->GetChildActor(), 0.1f);
		PointOfViewType = EPointOfViewType::EPT_ThirdPersonView;
		SetCharacterMoveStateUpdateSpeed(EMoveState::EMS_Walk);
	}
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
			Movement->MaxWalkSpeed = Movement->MaxWalkSpeedCrouched;
			break;
		case EMoveState::EMS_ZoomInWalk:
			Movement->MaxWalkSpeed = ZoomInWalkSpeed;
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
	if (CharacterMoveState == EMoveState::EMS_ZoomInWalk)
	{
		return;
	}
	
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
	if (CharacterMoveState == EMoveState::EMS_ZoomInWalk)
	{
		return;
	}
	
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
			SetCharacterMoveStateUpdateSpeed(EMoveState::EMS_Walk);
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
			EnhancedInputComponent->BindAction(InputDataAsset->ZoomInAction, ETriggerEvent::Started, this, &AFpsWaveCharacterController::ZoomInStarted);
			EnhancedInputComponent->BindAction(InputDataAsset->ZoomInAction, ETriggerEvent::Completed, this, &AFpsWaveCharacterController::ZoomInCompleted);
		}
	}
}



