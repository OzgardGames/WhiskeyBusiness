// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
struct FDamageInfo;

class QUETZALMULTIPLAYER_API IDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int GetHealth() = 0;
	virtual int GetStaggerHealth() = 0;
	virtual int GetMaxHealth() = 0;
	virtual int GetMaxStaggerHealth() = 0;
	virtual bool IsInvulnerable() = 0;
	//virtual bool IsDead() = 0; //in character state
	//virtual bool IsStunned() = 0;//in character state
	virtual bool IsBlocking() = 0;
	//virtual bool IsParrying() = 0; //TODO if we want parry

	virtual void SetMaxHealth(int setMaxHealth) = 0;
	virtual void SetHealth(int setHealth) = 0;
	virtual void SetStaggerHealth(int setStagger) = 0;
	virtual void SetMaxStaggerHealth(int setMaxStagger) = 0;
	virtual void SetIsInvulnerable(bool setInvulnerable) = 0;
	//virtual void SetIsDead(bool setDead) = 0;//in character state
	//virtual void SetIsStunned(bool setStunned) = 0;//in character state
	virtual void SetIsBlocking(bool setBlocking) = 0;
	//virtual void SetIsParrying(bool setParrying) = 0; //TODO if we want parry

	virtual int Heal(int healAmount) = 0;
	virtual bool TakeDamage(FDamageInfo damageInfo) = 0;

};
