/**
 * @file Projectile.h
 * @brief Projectile for ranged weapons.
 * @version 0.1
 * @date 2025-02-24
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "ProjectileRevolver.generated.h"
class ARevolver;
UCLASS()
class QUETZALMULTIPLAYER_API AProjectileRevolver : public AProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileRevolver();

	/** how many times does the projectile bounces*/
	int maxNumberOfBounces;

	/** flag to let the projectile hit the ignored actor after the first bounce*/
	bool hitIgnoredActor;

	/**
	* @brief Called when the weapon overlaps with another actor during a throw.
	* @param HitComponent The component that overlaps.
	* @param OtherActor The actor that overlaps.
	* @param OtherComp The component of the other actor.
	* @param NormalImpulse 
	* @param Hit 
	*/
	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};