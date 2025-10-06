// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Crosshair.h"

#include "Characters/FpsWaveCharacter.h"
#include "Components/Border.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "Weapon/FpsWaveWeapon.h"

void UCrosshair::NativeConstruct()
{
	Super::NativeConstruct();

	//GetWorld()->GetTimerManager().SetTimer(DelegateHandle, this, &UCrosshair::BindDelegates, 0.2f);

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
		if (AFpsWaveCharacterController* Controller = Cast<AFpsWaveCharacterController>(PC))
		{
			Controller->OnAttackDelegate.AddUObject(this, &UCrosshair::IncreaseAimWidth);
			Controller->OnAttackFinishedDelegate.AddUObject(this, &UCrosshair::ResetAimWidth);

			if (AFpsWaveCharacter* FpsWaveCharacter = Cast<AFpsWaveCharacter>(Controller->GetPawn()))
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
        if (CurrentAttackDelay <= 1e-12)
        {
            CurrentTopLocation.Y = FMath::FInterpTo(CurrentTopLocation.Y, MaxTopLocation.Y, InDeltaTime, 10.f);
            CurrentBottomLocation.Y = FMath::FInterpTo(CurrentBottomLocation.Y, MaxBottomLocation.Y, InDeltaTime, 10.f);
            CurrentLeftLocation.X = FMath::FInterpTo(CurrentLeftLocation.X, MaxLeftLocation.X, InDeltaTime, 10.f);
            CurrentRightLocation.X = FMath::FInterpTo(CurrentRightLocation.X, MaxRightLocation.X, InDeltaTime, 10.f);
        	CurrentAttackDelay = MaxAttackDelay;
        }
        
        UpdateCrosshairPositions();
    }
    else
    {
        // 공격이 완전히 끝났을 때 (연속 사격 아닐 때)
        if (!bAttackFinished)
        {
        	//todo interpspeed 변수화
            CurrentTopLocation.Y = FMath::FInterpTo(CurrentTopLocation.Y, DefaultTopLocation.Y, InDeltaTime, 5.f);
            CurrentBottomLocation.Y = FMath::FInterpTo(CurrentBottomLocation.Y, DefaultBottomLocation.Y, InDeltaTime, 5.f);
            CurrentLeftLocation.X = FMath::FInterpTo(CurrentLeftLocation.X, DefaultLeftLocation.X, InDeltaTime, 5.f);
            CurrentRightLocation.X = FMath::FInterpTo(CurrentRightLocation.X, DefaultRightLocation.X, InDeltaTime, 5.f);
            
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

void UCrosshair::IncreaseAimWidth()
{
	bAttackStarted = true;
	bAttackFinished = false;

	if (Player)
	{
		MaxAttackDelay = Player->GetEquippedWeapon()->GetAttackDelay();
		CurrentAttackDelay = 0.1f;
	}
}

void UCrosshair::ResetAimWidth()
{
	bAttackStarted = false;
}



