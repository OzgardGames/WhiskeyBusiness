// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "DamageResponse.generated.h"

UENUM(BlueprintType)
enum class EDamageResponse : uint8
{
	HIT_REACTION	UMETA(DisplayName = "Hit Reaction"),
	STUN			UMETA(DisplayName = "Stun"),
	KNOCK_BACK		UMETA(DisplayName = "Knock Back"),
	DANCE			UMETA(DisplayName = "Dance"),
	INSTANT_KILL	UMETA(DisplayName = "Instant Kill"),
	NONE			UMETA(DisplayName = "None"),
	_MAX			UMETA(Hidden)
};
