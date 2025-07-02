
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Drink/Drink.h"
#include "NitroShot.generated.h"



UCLASS()
class QUETZALMULTIPLAYER_API ANitroShot : public ADrink
{
	GENERATED_BODY()

public:
	
	ANitroShot();

	
	virtual void ApplyEffect(AQuetzalMultiplayerCharacter* Character) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink|Speed", meta = (ClampMin = "1.0"))
	float SpeedMultiplier;

	float OriginalMaxWalkSpeed;

	
	UPROPERTY()
	AQuetzalMultiplayerCharacter* AffectedCharacter;


	FTimerHandle SpeedEffectTimer;
};