/**
 * @file WeaponMelee.h
 * @brief Child class of Weapon, parent class of all melee weapons.
 * @version 0.1
 * @date 2025-02-02
 */

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponMelee.generated.h"

 /**
  * @class AWeaponMelee
  * @brief Child class of Weapon, parent class of all melee weapons.
  */

UCLASS()
class QUETZALMULTIPLAYER_API AWeaponMelee : public AWeapon
{
	GENERATED_BODY()
public:
	/** Hit result from @see CheckMeleeCollision()*/
	FHitResult hitResult;

	/** Hit result from @see CheckBetweenMeleeCollision()*/
	FHitResult hitResultTailEnd;

	/** when to check for melee collision, used for MeleeCollisionCheckNotify*/
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Weapon")
	bool bActivateWeaponCollisionCheck;
	
	AWeaponMelee();
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	* @brief Checks for melee collision.
	* @return The hit result of the collision check.
	*/
	virtual FHitResult CheckMeleeCollision();

	/**
	* @brief Checks for melee collision behind weapon, use this because of fast animations to not miss between frames.
	* @return The hit result of the collision check.
	*/
	virtual FHitResult CheckBetweenMeleeCollision();

public:
	UFUNCTION(Server, Reliable)
	void Server_SetWeaponCollisionCheck(bool bActive);
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWeaponCollisionCheck(bool bActive);
};
