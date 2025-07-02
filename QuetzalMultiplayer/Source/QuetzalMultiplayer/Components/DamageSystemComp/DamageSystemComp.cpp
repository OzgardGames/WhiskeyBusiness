// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DamageSystemComp/DamageSystemComp.h"
#include "Character/CharacterState.h"

// Sets default values for this component's properties
UDamageSystemComp::UDamageSystemComp():
	currentHealth(100),
	maxHealth(100),
	currentStagger(5),
	maxStagger(5),
	bIsInvulnerable(false),
	bIsDead(false),
	bIsStunned(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	this->OnBlock.Clear();
	this->OnStaggerDepleted.Clear();
	this->OnTriggerInvulnerable.Clear();
	this->OnDeath.Clear();
	this->OnDamageResponse.Clear();
	//SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UDamageSystemComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDamageSystemComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDamageSystemComp::SetMaxHealth(int setMaxHealth)
{
	maxHealth = setMaxHealth; 
	if (maxHealth < currentHealth) 
		SetHealth(maxHealth); 
}

void UDamageSystemComp::SetHealth(int setHealth)
{
	currentHealth = setHealth;
	if (currentHealth > maxHealth)
		currentHealth = maxHealth;
}

void UDamageSystemComp::SetStaggerHealth(int setStagger)
{
	currentStagger = setStagger;
	if (currentStagger > maxStagger)
		currentStagger = maxStagger;
}

void UDamageSystemComp::SetMaxStaggerHealth(int setMaxStagger)
{
	maxStagger = setMaxStagger;
	if (maxStagger < currentStagger)
		SetStaggerHealth(maxStagger);
}

int UDamageSystemComp::Heal(int healAmount)
{
	if(!bIsDead)
	{
		currentHealth += healAmount;
		if (currentHealth > maxHealth)
			currentHealth = maxHealth;
	}
	return currentHealth;
}

bool UDamageSystemComp::TakeDamage(FDamageInfo damageInfo)
{
	if (!bIsDead && (!bIsInvulnerable || damageInfo.ShouldDamageInvulnerable))
	{
		if (bIsBlocking && damageInfo.canBeBlocked)
		{
			OnBlock.Broadcast();
			return false;
		}
		currentHealth -= damageInfo.amountOfDamage;
		currentStagger -= damageInfo.amountOfStaggerDamage;
		OnDamageResponse.Broadcast(damageInfo.damageResponse, damageInfo);
		if (currentHealth <= 0 || damageInfo.damageResponse==EDamageResponse::INSTANT_KILL)
		{
			currentHealth = 0;
			bIsDead = true;
			OnDeath.Broadcast();//TODO using ECharacterState creates coupling
			return true;
		}
		else if (currentStagger<=0)
		{
			OnStaggerDepleted.Broadcast();
		}
		OnTriggerInvulnerable.Broadcast(damageInfo.ShouldTriggerInvulnerable);
		
		//TODO may need to move to this position OnDamageResponse.Broadcast(damageInfo.damageResponse); because of OnDeath broadcast...
		return true;
	}
	return false;
}

