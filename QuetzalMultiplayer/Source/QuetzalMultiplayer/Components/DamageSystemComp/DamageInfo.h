// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "CustomDamageType.h"
#include "DamageResponse.h"
#include "DamageInfo.generated.h"

class IDamageInterface;
USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int amountOfDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int amountOfStaggerDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldDamageInvulnerable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldTriggerInvulnerable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canBeBlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canBeParried;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool forceInterruptAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector impactForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECustomDamageType damageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageResponse damageResponse;

	IDamageInterface* DamageReceiver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* InstigatedBy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DamageCauser;

	FDamageInfo()
	{
		amountOfDamage = 0;
		amountOfStaggerDamage = 0;
		impactForce = FVector(0.0f);
		ShouldDamageInvulnerable = false;
		canBeBlocked = true;
		canBeParried = true;
		forceInterruptAnimation = false;
		ShouldTriggerInvulnerable = true;
		damageType = ECustomDamageType::NONE;
		damageResponse = EDamageResponse::NONE;
		DamageReceiver = nullptr;
		InstigatedBy = nullptr;
		DamageCauser = nullptr;
	}

};
