// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <DamageInfo.h>
#include "DamageSystemComp.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlock);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaggerDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerInvulnerable, bool, triggerInvulnerable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageResponse, EDamageResponse, damageResponse, FDamageInfo, damageInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUETZALMULTIPLAYER_API UDamageSystemComp : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int currentHealth;
	int maxHealth;
	int currentStagger;
	int maxStagger;
	bool bIsInvulnerable;
	bool bIsDead;
	bool bIsStunned;
	bool bIsBlocking;
	bool bIsParrying;

public:	
	UDamageSystemComp();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//GETTERS
	FORCEINLINE int  GetMaxHealth()	const		 { return maxHealth; };
	FORCEINLINE int  GetHealth() const			 { return currentHealth; };
	FORCEINLINE int  GetStaggerHealth()	const	 { return currentStagger; };
	FORCEINLINE int  GetMaxStaggerHealth() const { return maxStagger; };
	FORCEINLINE bool IsInvulnerable() const		 { return bIsInvulnerable; };
	FORCEINLINE bool IsDead() const				 { return bIsDead; };
	FORCEINLINE bool IsStunned() const			 { return bIsStunned; };
	FORCEINLINE bool IsBlocking() const			 { return bIsBlocking; };
	FORCEINLINE bool IsParrying() const			 { return bIsParrying; };

	//SETTERS
	void SetMaxHealth(int setMaxHealth);
	void SetHealth(int setHealth);
	void SetStaggerHealth(int setStagger);
	void SetMaxStaggerHealth(int setMaxStagger);
	FORCEINLINE void SetIsInvulnerable(bool setInvulnerable){ bIsInvulnerable = setInvulnerable; };
	FORCEINLINE void SetIsDead(bool setDead)				{ bIsDead = setDead; };
	FORCEINLINE void SetIsStunned(bool setStunned)			{ bIsStunned = setStunned; };
	FORCEINLINE void SetIsBlocking(bool setBlocking)		{ bIsBlocking = setBlocking; };
	FORCEINLINE void SetIsParrying(bool setParrying)		{ bIsParrying = setParrying; };
	
	int Heal(int healAmount);

	bool TakeDamage(FDamageInfo damageInfo);

	UPROPERTY(BlueprintAssignable, Category = "On Death Delegate")
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "On Damage Response Delegate")
	FOnDamageResponse OnDamageResponse;

	UPROPERTY(BlueprintAssignable, Category = "On block Delegate")
	FOnBlock OnBlock;

	UPROPERTY(BlueprintAssignable, Category = "On Stagger Depleted Delegate")
	FOnStaggerDepleted OnStaggerDepleted;

	UPROPERTY(BlueprintAssignable, Category = "On Trigger Invulnerable Delegate")
	FOnTriggerInvulnerable OnTriggerInvulnerable;
};
