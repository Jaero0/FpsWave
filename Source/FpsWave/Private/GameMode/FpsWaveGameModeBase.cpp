// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FpsWaveGameModeBase.h"
#include "Blueprint/UserWidget.h"

void AFpsWaveGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass)
	{
		if (UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WidgetClass))
		{
			UserWidget->AddToViewport();
		}
	}
}
