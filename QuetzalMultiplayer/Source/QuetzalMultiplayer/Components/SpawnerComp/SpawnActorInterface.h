// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpawnActorInterface.generated.h"

struct UStructSpawnLocationAndRotation
{
	/** Spawn Location of the weapon. */
	FVector spawnLocation;
	/** Spawn Rotation of the weapon. */
	FRotator spawnRotation;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USpawnActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QUETZALMULTIPLAYER_API ISpawnActorInterface
{
	GENERATED_BODY()
protected:
	/** Weapon needs to be Respawned */
	bool bRespawnFlag;

	float respawnTime;
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UStructSpawnLocationAndRotation originalSpawnLocationAndRotation;
	//Setters
	virtual void SetRespawnTime(float time) = 0;
	virtual void SetRespawnFlag(bool respawn) = 0;

	//Getters
	virtual float GetRespawnTime() = 0;
	virtual bool GetRespawnFlag() = 0;


	virtual void RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation) = 0;

};
