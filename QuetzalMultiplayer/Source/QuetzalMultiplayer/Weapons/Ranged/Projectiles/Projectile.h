/**
 * @file Projectile.h
 * @brief Projectile for ranged weapons.
 * @version 0.1
 * @date 2025-02-24
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"
class AWeaponRanged;
UCLASS()
class QUETZALMULTIPLAYER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Static Mesh used to provide a visual representation of the object.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* StaticMesh;
	// Sphere component used to test collision.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComponent;
	// Movement component for handling projectile movement.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
	/** Pointer to the weapon that shot the projectile*/
	AWeaponRanged* owernerOfProjectile;

	// Sets default values for this actor's properties
	AProjectile();

	/**
	* @brief Called when the weapon overlaps with another actor during a throw.
	* @param HitComponent The component that overlaps.
	* @param OtherActor The actor that overlaps.
	* @param OtherComp The component of the other actor.
	* @param NormalImpulse 
	* @param Hit 
	*/
	UFUNCTION()
	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
