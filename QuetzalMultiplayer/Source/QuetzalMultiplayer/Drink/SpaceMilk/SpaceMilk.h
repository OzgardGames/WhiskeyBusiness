// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drink/Drink.h"
#include "QuetzalMultiplayerCharacter.h"
#include "SpaceMilk.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API ASpaceMilk : public ADrink
{
	GENERATED_BODY()

public:

	ASpaceMilk();

	
	virtual void ApplyEffect(AQuetzalMultiplayerCharacter* Character) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink|Jump", meta = (ClampMin = "1.0"))
	float JumpHeightMultiplier;
	float playerInitialJump;
	
};
