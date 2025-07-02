// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Drink/Drink.h"
#include "QuetzalMultiplayerCharacter.h"
#include "TimeWarpWine.generated.h"


UCLASS()
class QUETZALMULTIPLAYER_API ATimeWarpWine : public ADrink
{
	GENERATED_BODY()

public:

	ATimeWarpWine();

	
	virtual void ApplyEffect(AQuetzalMultiplayerCharacter* Character) override;


protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink|TimeEffect", meta = (ClampMin = "1.1", ClampMax = "5.0"))
	float TimeDilationFactor;
	float OriginalMaxWalkSpeed;
};