// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Melee/WeaponMelee.h"

#include <Components/AttackSystemComp/CustomRadialForceComponent.h>

#include "Mallet.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API AMallet : public AWeaponMelee
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInstance* spinCooldownHightlightColor;
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInterface* originalHightlightColor;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCustomRadialForceComponent* RadialForceComp;


protected:
	/** Speed of the spin rotation*/
	float spinSpeed;
	/** How long can a player spin*/
	float spinDuration;
	/** How long before a player can spin again*/
	float spinCooldown;
	/** Flag to make sure the impulse from hidding a wall only happens 1 time*/
	bool impulseFired;
	/** spin angle used with delta time to rotate the player*/
	float spinAngle;
	/** Timer for cooldown*/
	FTimerHandle spinAttackCooldown;
	/** Timer for special attack duration*/
	FTimerHandle spinAttackDuration;

public:
	AMallet();
	virtual void Tick(float deltaTime) override;

protected:
	/**
	 * @brief Called when the player picks up the weapon.
	 * @param the player that picked up the weapon.
	 */
	virtual void OnPickup(AQuetzalMultiplayerCharacter* pickedUpActor) override;
	
	/**
	* @brief Triggers basic attack.
	* @param bButtonHeld if left click is helddown
	*/
	virtual void TriggerBasicAttack(bool bButtonHeld) override;
	
	/**
	* @brief Triggers special attack.
	* @param bButtonHeld if right click is helddown
	*/
	virtual void TriggerSpecialAttack(bool bButtonHeld) override;

	/**
	* @brief Triggers timer for the Duration the player can spin.
	*/
	void triggerSpinAttackDuration();

	/**
	* @brief Triggers timer for the cooldown for when they can spin again.
	*/
	void triggerSpinAttackCooldown();

	/**
	* @brief start glowing weapon based on how long they held the special attack down
	*/
	void spinAttackCooldownColorLogic();

	/**
	* @brief Triggers stop spin.
	*/
	void triggerStopSpinAttack();

	/**
	* @brief check if mallet hit the floor and trigger impulse.
	*/
	UFUNCTION()
	void checkFloorCollision();

	/**
	* @brief Set the Mallet Color
	* @param color
	*/
	void setMalletColor(FLinearColor color);
	UFUNCTION(Server, Reliable)
	void Server_setMalletColor(FLinearColor color);
	UFUNCTION(NetMulticast, Reliable)
	void MC_setMalletColor(FLinearColor color);

	/**
	* @brief Triggers special attack animation and replicate
	* @param bButtonHeld if right click is helddown
	*/
	UFUNCTION(Server, Reliable)
	void Server_Spin(bool bButtonHeld);
	UFUNCTION(NetMulticast, Reliable)
	void MC_Spin(bool bButtonHeld);

	/**
	* @brief Throws the weapon in the specified direction.
	* @param Direction The direction to throw the weapon.
	*/
	virtual void ThrowWeapon(FVector Direction) override;
	UFUNCTION(Server, Reliable)
	void Server_ThrowMallet(FVector Direction);
	UFUNCTION(NetMulticast, Reliable)
	void MC_ThrowMallet(FVector Direction);

	/**
	* @brief Called to drop the equiped weapon.
	*/
	virtual void DropWeapon() override;
	UFUNCTION(Server, Reliable)
	void Server_DropMallet();
	UFUNCTION(NetMulticast, Reliable)
	void MC_DropMallet();

	virtual void SetupDefaultDamageInfo() override;
};
