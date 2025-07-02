// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingBrew.h"
#include "QuetzalMultiplayerCharacter.h"

AHealingBrew::AHealingBrew()
{
	PrimaryActorTick.bCanEverTick = true;

	DrinkName = TEXT("HealingBrew");
	DrinkType = EDrinkType::HealingBrew;
	EffectDuration = 0.0f;  
	HealthAmount = 25;
}

void AHealingBrew::ApplyEffect(AQuetzalMultiplayerCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	
	int32 actualHealAmount = Character->Heal(HealthAmount);

	
	

	Super::ApplyEffect(Character);
}

void AHealingBrew::ConsumeDrink()
{
	
	//Destroy();
}


