// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drink/Drink.h"	
#include "HealingBrew.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API AHealingBrew : public ADrink
{
	GENERATED_BODY()

public:
	
	AHealingBrew();

	
	virtual void ApplyEffect(AQuetzalMultiplayerCharacter* Character) override;

	
	virtual void ConsumeDrink() override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink|Health")
	float HealthAmount;

};
