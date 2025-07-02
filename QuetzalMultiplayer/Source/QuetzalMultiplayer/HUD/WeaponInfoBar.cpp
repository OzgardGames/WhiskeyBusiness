// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeaponInfoBar.h"
#include "Components/ProgressBar.h"
void UWeaponInfoBar::NativeConstruct()
{
	Super::NativeConstruct();
	UProgressBar* progressBarWidget = Cast<UProgressBar>(GetWidgetFromName("WG_Character_WeaponInfo"));
	
}

float UWeaponInfoBar::CalculateInfoPercentage(float currentValue, float MaxValue)
{

	return currentValue / MaxValue;

	
}
