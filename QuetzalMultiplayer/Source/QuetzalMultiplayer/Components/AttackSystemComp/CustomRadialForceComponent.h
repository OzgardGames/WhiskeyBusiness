// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "CustomRadialForceComponent.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API UCustomRadialForceComponent : public URadialForceComponent
{
	GENERATED_BODY()
	
public:
	/** Display debug message*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DEBUG_TRACE_RADIUS = false;
	TArray<AActor*> actorsToIgnore;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void FireImpulse() override;
};
