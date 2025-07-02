/**
 * @file Revolver.h
 * @brief Revolver ranged type weapon, child of AWeaponRanged.
 * @version 0.4
 * @date 2025-02-02
 *
 *	Change Log:
 *	20250224-FB-v0.2: added spawned Projectile at the end of the ranged attack animation
 *  20250320-FB-v0.3: changed how trigger attack is handled
 *	20250322-FB-v0.4: added revolver animations
 */
#pragma once

#include "CoreMinimal.h"
#include "WeaponRanged.h"
#include "Revolver.generated.h"

const int WEAPON_REVOLVER_MAX_AMMO = 6;
UCLASS()
class QUETZALMULTIPLAYER_API ARevolver : public AWeaponRanged
{
	GENERATED_BODY()
public:
	ARevolver();
protected:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectileRevolver> ProjectileClass;
	
	FRotator originalRotation;
	FRotator currentRotation;
	float attackAngle;
	/** cooldown for rightclick attack. */
	FTimerHandle revolverSpecialTimer;

	/**
	* @brief if the weapon has ammo, create projectile and launch in the forward direction.
	* @param bButtonHeld if left click is helddown
	*/
	virtual void TriggerBasicAttack(bool bButtonHeld) override;
	/**
	* @brief TODO currently no special for revolver, it will call @see TriggerBasicAttack()
	* @param bButtonHeld if right click is helddown
	*/
	virtual void TriggerSpecialAttack(bool bButtonHeld) override;
public:
	/**
	* @brief if the weapon has ammo, create projectile and launch in the forward direction.
	* @param HitComponent The component that overlaps.
	* @param OtherActor The actor that overlaps.
	* @param OtherComp The component of the other actor.
	* @param NormalImpulse The body index of the other actor.
	* @param Hit Structure containing information about one hit of a trace, such as point of impact and surface normal at that point.
	*/
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation) override;
	UFUNCTION(Server, Reliable)
	void Server_RespawnRevolver(FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(NetMulticast, Reliable)
	void MC_RespawnRevolver(FVector spawnLocation, FRotator spawnRotation);
protected:
	/**
	* @brief Adjust Laser Distance off of the first target hit
	*/
	virtual void AdjustLaserDistance();

	UFUNCTION(Server, Reliable)
	void Server_BasicAttack(bool bButtonHeld);

	UFUNCTION(NetMulticast, Reliable)
	void MC_BasicAttack(bool bButtonHeld);

	virtual void SetupDefaultDamageInfo() override;

	void RevolverSpecialAttack();

	virtual void BeginPlay() override;

};
