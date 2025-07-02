// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "CustomDamageType.generated.h"

UENUM(BlueprintType)
enum class ECustomDamageType : uint8
{
	LIGHT_MELEE		UMETA(DisplayName = "Light melee"),
	LIGHT_RANGED	UMETA(DisplayName = "Light ranged"),
	HEAVY_MELEE		UMETA(DisplayName = "Heavy melee"),
	HEAVY_RANGED	UMETA(DisplayName = "Heavy ranged"),
	THROW_WEAPON	UMETA(DisplayName = "Throw weapon"),
	EXPLOSION		UMETA(DisplayName = "Explosion"),
	ENVIRONMENTAL	UMETA(DisplayName = "Environmental"),
	IMPACT			UMETA(DisplayName = "Impact"),
	NONE			UMETA(DisplayName = "None"),
	_MAX			UMETA(Hidden)
};
