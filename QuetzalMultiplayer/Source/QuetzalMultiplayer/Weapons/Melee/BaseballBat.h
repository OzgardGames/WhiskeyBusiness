/****************************************************************************
*	Name: BaseballBat
*	Version: 0.1
*	Created: 2025/02/02
*	Description:
*	- child class of WeaponMelee
*	- melee weapon Baseball bat
*	Change Log:
****************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "WeaponMelee.h"
#include "BaseballBat.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API ABaseballBat : public AWeaponMelee
{
	GENERATED_BODY()
public:
	ABaseballBat();
	virtual void Tick(float deltaTime)override;

protected:
	/** Timer for special attack, longer the player holds the more damage and push back, uses delta time*/
	float chargeAttackTimer;
	/** Flag when the weapon is at max charge*/
	bool maxChargeFlash;
	/** Holds the Max Charge Value (Default 3.f)*/
	float maxCharge;
	/** The Color of the bat while it's charging*/
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInstance* chargehightlightColor;
	/** Original Color of the bat*/
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInterface* originalhightlightColor;


	/**
	* @brief Triggers basic attack.
	* @param bButtonHeld if left click is helddown
	*/
	virtual void TriggerBasicAttack(bool bButtonHeld) override;
	UFUNCTION(Server, Reliable)
	void Server_BasicAttack(bool bButtonHeld);
	UFUNCTION(NetMulticast, Reliable)
	void MC_BasicAttack(bool bButtonHeld);
	
	/**
	* @brief Triggers special attack.
	* @param bButtonHeld if right click is helddown
	*/
	virtual void TriggerSpecialAttack(bool bButtonHeld) override;
	UFUNCTION(Server, Reliable)
	void Server_SpecialAttack(bool bButtonHeld);
	UFUNCTION(NetMulticast, Reliable)
	void MC_SpecialAttack(bool bButtonHeld);
	
	/**
	* @brief Set the Bat Color based on chargeAttackTimer
	* @param deltaTime
	*/
	void chargeAttackColorLogic(float deltaTime);

	/**
	* @brief Throws the weapon in the specified direction.
	* @param Direction The direction to throw the weapon.
	*/
	virtual void ThrowWeapon(FVector Direction) override;
	UFUNCTION(Server, Reliable)
	void Server_ThrowBaseballBat(FVector Direction);
	UFUNCTION(NetMulticast, Reliable)
	void MC_ThrowBaseballBat(FVector Direction);

	/**
	* @brief Called to drop the equiped weapon.
	*/
	virtual void DropWeapon() override;
	UFUNCTION(Server, Reliable)
	void Server_DropBaseballBat();
	UFUNCTION(NetMulticast, Reliable)
	void MC_DropBaseballBat();
	
	virtual void SetupDefaultDamageInfo() override;
};
