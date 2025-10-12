// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Crosshair.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Characters/FpsWaveCharacter.h"
#include "Components/Border.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "Weapon/FpsWaveWeapon.h"

void UCrosshair::NativeConstruct()
{
	Super::NativeConstruct();
	
	DefaultTopLocation = Top->GetRenderTransform().Translation;
	CurrentTopLocation = DefaultTopLocation;
	PostTopLocation = CurrentTopLocation;
	MaxTopLocation = DefaultTopLocation;
	MaxTopLocation.Y -= LimitAmount;
	DefaultBottomLocation = Bottom->GetRenderTransform().Translation;
	CurrentBottomLocation = DefaultBottomLocation;
	PostBottomLocation = CurrentBottomLocation;
	MaxBottomLocation = DefaultBottomLocation;
	MaxBottomLocation.Y += LimitAmount;
	DefaultLeftLocation = Left->GetRenderTransform().Translation;
	CurrentLeftLocation = DefaultLeftLocation;
	PostLeftLocation = CurrentLeftLocation;
	MaxLeftLocation = DefaultLeftLocation;
	MaxLeftLocation.X -= LimitAmount;
	DefaultRightLocation = Right->GetRenderTransform().Translation;
	CurrentRightLocation = DefaultRightLocation;
	PostRightLocation = CurrentRightLocation;
	MaxRightLocation = DefaultRightLocation;
	MaxRightLocation.X += LimitAmount;

	GetWorld()->GetTimerManager().SetTimer(DelegateHandle,this, &UCrosshair::BindDelegates,0.2f);
}

void UCrosshair::BindDelegates()
{
	if (AFpsWaveCharacterController* PC = GetWorld()->GetFirstPlayerController<AFpsWaveCharacterController>())
	{
		PlayerController = PC;
		
		if (AFpsWaveCharacter* FpsWaveCharacter = Cast<AFpsWaveCharacter>(PC->GetPawn()))
		{
			Player = FpsWaveCharacter;
			Player->OnWeaponChangeDelegate.AddUObject(this, &UCrosshair::ChangeDelegate);
			if (auto Weapon = Player->GetEquippedWeapon())
			{
				CurrentDelegateHandle = Weapon->OnAttackDelegate.AddUObject(this, &UCrosshair::OnAttackDelegate);
			}
		}
	}
}

void UCrosshair::ChangeDelegate(TObjectPtr<class AFpsWaveWeapon> Weapon)
{
	Weapon->OnAttackDelegate.AddUObject(this, &UCrosshair::OnAttackDelegate);
}

void UCrosshair::UpdateCrosshairPositions()
{
	Top->SetRenderTranslation(CurrentTopLocation);
	Bottom->SetRenderTranslation(CurrentBottomLocation);
	Left->SetRenderTranslation(CurrentLeftLocation);
	Right->SetRenderTranslation(CurrentRightLocation);
}

void UCrosshair::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    //공격시 todo
    if (CrosshairState == ECrosshairState::ECS_Increasing)
    {
    	CurrentTopLocation.Y = FMath::FInterpTo(CurrentTopLocation.Y, PostTopLocation.Y, InDeltaTime,
											AimIncreaseSpeed);
    	CurrentBottomLocation.Y = FMath::FInterpTo(CurrentBottomLocation.Y, PostBottomLocation.Y, InDeltaTime,
												   AimIncreaseSpeed);
    	CurrentLeftLocation.X = FMath::FInterpTo(CurrentLeftLocation.X, PostLeftLocation.X, InDeltaTime,
												 AimIncreaseSpeed);
    	CurrentRightLocation.X = FMath::FInterpTo(CurrentRightLocation.X, PostRightLocation.X, InDeltaTime,
												  AimIncreaseSpeed);
    	UpdateCrosshairPositions();
    }

	float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastIncreaseTime > AttackDelay && CrosshairState == ECrosshairState::ECS_Increasing)
    {
	    CrosshairState = ECrosshairState::ECS_Decreasing;
    }

    if (CrosshairState == ECrosshairState::ECS_Idle)
    {
	    return;
    }

    if (CrosshairState == ECrosshairState::ECS_Decreasing)
    {
    	CurrentTopLocation.Y = FMath::FInterpTo(CurrentTopLocation.Y, DefaultTopLocation.Y, InDeltaTime,
												AimDecreaseSpeed);
    	CurrentBottomLocation.Y = FMath::FInterpTo(CurrentBottomLocation.Y, DefaultBottomLocation.Y, InDeltaTime,
												   AimDecreaseSpeed);
    	CurrentLeftLocation.X = FMath::FInterpTo(CurrentLeftLocation.X, DefaultLeftLocation.X, InDeltaTime,
												 AimDecreaseSpeed);
    	CurrentRightLocation.X = FMath::FInterpTo(CurrentRightLocation.X, DefaultRightLocation.X, InDeltaTime,
												  AimDecreaseSpeed);

    	if (IsCurrentLocationNearToDefaultLocation())
    	{
    		CurrentTopLocation = DefaultTopLocation;
    		CurrentBottomLocation = DefaultBottomLocation;
    		CurrentLeftLocation = DefaultLeftLocation;
    		CurrentRightLocation = DefaultRightLocation;
    		CrosshairState = ECrosshairState::ECS_Idle;
    	}

    	UpdateCrosshairPositions();
    }
}

//todo delegate 등록 후 
void UCrosshair::OnAttackDelegate()
{
	CrosshairState = ECrosshairState::ECS_Increasing;
	LastIncreaseTime = GetWorld()->GetTimeSeconds();
	
	if (Player)
	{
		float GunIncreaseSpeed = LimitAmount / (Player->GetEquippedWeapon()->GetMaxBulletCount() / 2) * 2;
		AttackDelay = Player->GetEquippedWeapon()->GetAttackDelay() + 0.3f;
		
		switch (Player->GetPlayerWeaponType())
		{
		case EPlayerWeaponType::EPW_Rifle:
			SetAimIncreaseSpeed(GunIncreaseSpeed);
			SetAimDecreaseSpeed(6.f);
			break;
		case EPlayerWeaponType::EPW_Shotgun:
			SetAimIncreaseSpeed(GunIncreaseSpeed);
			SetAimDecreaseSpeed(6.f);
			break;
		case EPlayerWeaponType::EPW_Katana:
			SetAimIncreaseSpeed(0);
			SetAimDecreaseSpeed(0);
			break;
		case EPlayerWeaponType::EPW_WarHammer:
			SetAimIncreaseSpeed(0);
			SetAimDecreaseSpeed(0);
			break;
		}
	}

	PostTopLocation.Y = FMath::Max(CurrentTopLocation.Y - AimIncreaseSpeed, MaxTopLocation.Y);
	PostBottomLocation.Y = FMath::Min(CurrentBottomLocation.Y + AimIncreaseSpeed, MaxBottomLocation.Y);
	PostLeftLocation.X = FMath::Max(CurrentLeftLocation.X - AimIncreaseSpeed, MaxLeftLocation.X);
	PostRightLocation.X = FMath::Min(CurrentRightLocation.X + AimIncreaseSpeed, MaxRightLocation.X);
}

FVector2d UCrosshair::GetAimLocation()
{
	FGeometry Geometry = Aim->GetCachedGeometry();
    
	FVector2D AbsolutePosition = Geometry.GetAbsolutePosition();
	FVector2D WidgetSize = Geometry.GetAbsoluteSize(); // 위젯 크기 가져오기
    
	// 중심점 계산
	FVector2D AbsoluteCenterPosition = AbsolutePosition + WidgetSize * 0.5f;
    
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
    
	USlateBlueprintLibrary::AbsoluteToViewport(
	   GetWorld(),
	   AbsoluteCenterPosition, // 중심점 사용
	   PixelPosition,
	   ViewportPosition
	);
    
	return PixelPosition;
}

bool UCrosshair::IsCurrentLocationNearToDefaultLocation()
{
	return FMath::IsNearlyEqual(CurrentTopLocation.Y, DefaultTopLocation.Y, 0.5f) &&
		FMath::IsNearlyEqual(CurrentBottomLocation.Y, DefaultBottomLocation.Y, 0.5f) &&
		FMath::IsNearlyEqual(CurrentLeftLocation.X, DefaultLeftLocation.X, 0.5f) &&
		FMath::IsNearlyEqual(CurrentRightLocation.X, DefaultRightLocation.X, 0.5f);
}

bool UCrosshair::IsCurrentLocationNearToMaxLocation()
{
	return FMath::IsNearlyEqual(CurrentTopLocation.Y, MaxTopLocation.Y, 0.5f) &&
		FMath::IsNearlyEqual(CurrentBottomLocation.Y, MaxBottomLocation.Y, 0.5f) &&
		FMath::IsNearlyEqual(CurrentLeftLocation.X, MaxLeftLocation.X, 0.5f) &&
		FMath::IsNearlyEqual(CurrentRightLocation.X, MaxRightLocation.X, 0.5f);
}

void UCrosshair::SetAimIncreaseSpeed(float Speed)
{
	AimIncreaseSpeed = Speed;
}

void UCrosshair::SetAimDecreaseSpeed(float Speed)
{
	AimDecreaseSpeed = Speed;
}


