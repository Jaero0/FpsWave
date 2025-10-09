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

	BindDelegates();
	
	DefaultTopLocation = Top->GetRenderTransform().Translation;
	CurrentTopLocation = DefaultTopLocation;
	MaxTopLocation = DefaultTopLocation;
	MaxTopLocation.Y -= 50.f;
	DefaultBottomLocation = Bottom->GetRenderTransform().Translation;
	CurrentBottomLocation = DefaultBottomLocation;
	MaxBottomLocation = DefaultBottomLocation;
	MaxBottomLocation.Y += 50.f;
	DefaultLeftLocation = Left->GetRenderTransform().Translation;
	CurrentLeftLocation = DefaultLeftLocation;
	MaxLeftLocation = DefaultLeftLocation;
	MaxLeftLocation.X -= 50.f;
	DefaultRightLocation = Right->GetRenderTransform().Translation;
	CurrentRightLocation = DefaultRightLocation;
	MaxRightLocation = DefaultRightLocation;
	MaxRightLocation.X += 50.f;
}

void UCrosshair::BindDelegates()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PlayerController = Cast<AFpsWaveCharacterController>(PC))
		{
			PlayerController->OnAttackDelegate.AddUObject(this, &UCrosshair::IncreaseAimWidth);
			PlayerController->OnAttackFinishedDelegate.AddUObject(this, &UCrosshair::ResetAimWidth);

			if (AFpsWaveCharacter* FpsWaveCharacter = Cast<AFpsWaveCharacter>(PlayerController->GetPawn()))
			{
				Player = FpsWaveCharacter;
			}
		}
	}
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

    if (bAttackStarted)
    {
        // Pending 시간 카운트
        CurrentAttackDelay -= InDeltaTime;

    	//공격시
        if (CurrentAttackDelay <= 0.f)
        {
            CurrentTopLocation.Y = FMath::FInterpTo(CurrentTopLocation.Y, MaxTopLocation.Y, InDeltaTime, AimIncreaseSpeed);
            CurrentBottomLocation.Y = FMath::FInterpTo(CurrentBottomLocation.Y, MaxBottomLocation.Y, InDeltaTime, AimIncreaseSpeed);
            CurrentLeftLocation.X = FMath::FInterpTo(CurrentLeftLocation.X, MaxLeftLocation.X, InDeltaTime, AimIncreaseSpeed);
            CurrentRightLocation.X = FMath::FInterpTo(CurrentRightLocation.X, MaxRightLocation.X, InDeltaTime, AimIncreaseSpeed);
        	CurrentAttackDelay = MaxAttackDelay;
        }
    	UpdateCrosshairPositions();
    }
    else
    {
        // 공격이 완전히 끝났을 때 (연속 사격 아닐 때)
        if (!bAttackFinished)
        {
            CurrentTopLocation.Y = FMath::FInterpTo(CurrentTopLocation.Y, DefaultTopLocation.Y, InDeltaTime, AimDecreaseSpeed);
            CurrentBottomLocation.Y = FMath::FInterpTo(CurrentBottomLocation.Y, DefaultBottomLocation.Y, InDeltaTime, AimDecreaseSpeed);
            CurrentLeftLocation.X = FMath::FInterpTo(CurrentLeftLocation.X, DefaultLeftLocation.X, InDeltaTime, AimDecreaseSpeed);
            CurrentRightLocation.X = FMath::FInterpTo(CurrentRightLocation.X, DefaultRightLocation.X, InDeltaTime, AimDecreaseSpeed);
            
            if (FMath::IsNearlyEqual(CurrentTopLocation.Y, DefaultTopLocation.Y, 0.5f) &&
                FMath::IsNearlyEqual(CurrentBottomLocation.Y, DefaultBottomLocation.Y, 0.5f) &&
                FMath::IsNearlyEqual(CurrentLeftLocation.X, DefaultLeftLocation.X, 0.5f) &&
                FMath::IsNearlyEqual(CurrentRightLocation.X, DefaultRightLocation.X, 0.5f))
            {
                bAttackFinished = true;
                CurrentTopLocation = DefaultTopLocation;
                CurrentBottomLocation = DefaultBottomLocation;
                CurrentLeftLocation = DefaultLeftLocation;
                CurrentRightLocation = DefaultRightLocation;
            }

            UpdateCrosshairPositions();
        }
    }
}

//todo 재장전시 prevnet
void UCrosshair::IncreaseAimWidth()
{
	bAttackStarted = true;
	bAttackFinished = false;

	if (Player)
	{
		MaxAttackDelay = Player->GetEquippedWeapon()->GetAttackDelay();
		CurrentAttackDelay = -1;

		switch (Player->GetPlayerWeaponType())
		{
		case EPlayerWeaponType::EPW_Rifle:
			SetAimIncreaseSpeed(10.f);
			SetAimDecreaseSpeed(5.f);
			break;
		case EPlayerWeaponType::EPW_Shotgun:
			SetAimIncreaseSpeed(30.f);
			SetAimDecreaseSpeed(10.f);
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
}

void UCrosshair::ResetAimWidth()
{
	bAttackStarted = false;
}

FVector2d UCrosshair::GetAimLocation()
{
	FGeometry Geometry = Aim->GetCachedGeometry();
    
	FVector2D AbsolutePosition = Geometry.GetAbsolutePosition();
	FVector2D WidgetSize = Geometry.GetAbsoluteSize(); // 위젯 크기 가져오기
    
	// 중심점 계산
	FVector2D AbsoluteCenterPosition = AbsolutePosition + (WidgetSize * 0.5f);
    
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

void UCrosshair::SetAimIncreaseSpeed(float Speed)
{
	AimIncreaseSpeed = Speed;
}

void UCrosshair::SetAimDecreaseSpeed(float Speed)
{
	AimDecreaseSpeed = Speed;
}


