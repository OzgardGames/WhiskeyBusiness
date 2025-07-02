// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoBar.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API UWeaponInfoBar : public UUserWidget
{
	GENERATED_BODY()
protected:
	// Begin Play alternative
	virtual void NativeConstruct() override;

public:
	/** Function to calculate Player HP Percentage */
	UFUNCTION(BlueprintPure)
	float CalculateInfoPercentage(float currentValue=0.1f,float MaxValue = 0.1f);

};
