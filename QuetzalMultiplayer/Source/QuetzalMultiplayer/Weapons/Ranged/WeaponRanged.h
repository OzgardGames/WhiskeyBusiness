/****************************************************************************
*	Name: WeaponRanged
*	Version: 0.2
*	Created: 2025/02/02
*	Description:
*	- child class of Weapon.
*	- parent class of all ranged weapons.
*	Change Log:
*	20250302-FB-v0.2: added ammo and Shooting function
****************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponRanged.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API AWeaponRanged : public AWeapon
{
	GENERATED_BODY()

public:
	AWeaponRanged();

protected:
	/** the Current ammo of the ranged weapon*/
	int currentAmmo;

	/** Name of the Muzzle socket. */
	UPROPERTY(EditAnywhere, Category = Socket)
	FName muzzleSocketName;
public:
	/**
	* @brief Called when the Projectile hits with another actor.
	* @param HitComponent The component that overlaps.
	* @param OtherActor The actor that overlaps.
	* @param OtherComp The component of the other actor.
	* @param NormalImpulse The body index of the other actor.
	* @param Hit Structure containing information about one hit of a trace, such as point of impact and surface normal at that point.
	*/
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
