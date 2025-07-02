/**
 * @file Weapon.h
 * @brief Parent class of all weapons.
 * @version 0.3
 * @date 2025-02-02
 *
 * Handles collision for pickup, attaches weapon to actor socket on overlap,
 * and throws weapon by detaching from actor and applying force.
 * 
 *	Change Log:
 *	20250224-FB-v0.2: added Aiming with a laser pointer
 * 	20250302-FB-v0.3: changed laser pointer to draw line for more accuracy
 *  20250322-FB-v0.4: Created Delegates for attack and special attack
 *  20250323-FB-v0.5: Creted Timers for attacks for cooldowns and to help with the animation lock
 */

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include <DamageInterface.h>
#include "DamageInfo.h"
#include "Components/SpawnerComp/SpawnActorInterface.h"
#include "Weapon.generated.h"

const float WEAPON_PICKUP_VISIBILITY_DURATION = 0.05f;
const float WEAPON_PICKUP_DELAY_DURATION = 1.00f;

/**  Weapon Rarity, number is used for the spawnTimer, bigger number longer time to spawn*/
UENUM(BlueprintType, Meta = (ScriptName = "EWeaponRarity"))
enum class EWeaponRarity : uint8
{
	COMMON		= 10	UMETA(DisplayName = "Common_Green"),
	RARE		= 20	UMETA(DisplayName = "Rare_Blue"),
	EPIC		= 30	UMETA(DisplayName = "Epic_Purple"),
	LENGENDARY	= 40	UMETA(DisplayName = "Lengendary_Orange"),
	NONE		= 0		UMETA(DisplayName = "None"),
	_MAX				UMETA(Hidden)
};

class USphereComponent;
class USkeletalMeshComponent;
class AQuetzalMultiplayerCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttack, bool, bButtonHeld);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecialAttack,bool,bButtonHeld);
/**
 * @class AWeapon
 * @brief Parent class of all weapons.
 */
UCLASS()
class QUETZALMULTIPLAYER_API AWeapon : public AActor, public ISpawnActorInterface
{
	GENERATED_BODY()


	/**************Variables**************************/
public:
	/** The actor to which the weapon is attached. */
	UPROPERTY(Replicated)
	AQuetzalMultiplayerCharacter* attachedActor;
	/** Indicates if the weapon is ranged. */
	UPROPERTY(EditAnywhere, Category = WeaponType)
	bool isRangedWeapon;
	/** Delegate for leftclick attack */
	UPROPERTY(BlueprintAssignable, Category = "On Attack")
	FOnAttack OnAttack;

	/** Delegate for rightclick attack */
	UPROPERTY(BlueprintAssignable, Category = "On Special Attack")
	FOnSpecialAttack OnSpecialAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool forceSpawnWeapon;

protected:
	/** Location of the weapon. */
	FVector location;

	/** cooldown for leftclick attack. */
	FTimerHandle attackCooldown;

	/** cooldown for rightclick attack. */
	FTimerHandle specialAttackCooldown;

	/** cooldown for leftclick attack. */
	FTimerHandle attackAnimationStopTimer;

	/** Timer Delegate for clearing attack timer. */
	FTimerDelegate TimerDel;

	/** Timer countdown for when the player cn pickup the weapon*/
	UPROPERTY(Replicated)
	float weaponPickupTimer;

	/** Timer for flashing weapon*/
	float flashPickUpTimer;

	/** flag when the player holds leftclick*/
	bool bAttackButtonHeld;

	/** flag when the player holds rightclick*/
	bool bSpecialAttackButtonHeld;

	/** location where the weapon floats up and down*/
	FVector UpDownLocation;

	/** timer for weapon floats up and down*/
	float weaponFloatingTimer;

	/** Indicates if the weapon is one-handed. */
	bool isOneHandWeapon;

	/** Indicates if the weapon is equipped. */
	bool isEquiped;

	/** Skeletal mesh component for the weapon. */
	UPROPERTY(EditAnywhere, Category = Mesh)
	USkeletalMeshComponent* skeletalMesh;

	/** Sphere collision component for pickup. */
	UPROPERTY(EditAnywhere, Category = "PickupCollision")
	USphereComponent* pickupCollisionComp;

	/** Sphere collision component for throw. */
	UPROPERTY(EditAnywhere, Category = "ThrowCollision")
	USphereComponent* throwCollisionComp;

	/** Name of the right hand socket. */
	UPROPERTY(EditAnywhere, Category = Socket)
	FName rightHandSocketName;

	/** Name of the left hand socket. */
	UPROPERTY(EditAnywhere, Category = Socket)
	FName leftHandSocketName;

	/** Display debug message*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DEBUG_TRACE_WEAPON_MELEE;

	/** Information of the amount and type of damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FDamageInfo damageInfo;

	/** Information of the rarity of the weapon */
	EWeaponRarity weaponRarity;

	/**************Variables**************************/

	/**************Functions**************************/
public:
	AWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	EWeaponRarity GetWeaponRarity();

	void WeaponFLoatingLogic(float deltaTime);

	void ActivateActor(bool active);
protected:
	UFUNCTION(Server, Reliable)
	void Server_ActivateActor(bool active);
	UFUNCTION(NetMulticast, Reliable)
	void MC_ActivateActor(bool active);

public:
	/**
	* @brief Throws the weapon in the specified direction.
	* @param Direction The direction to throw the weapon.
	*/
	virtual void ThrowWeapon(FVector Direction);

protected:
	UFUNCTION(Server, Reliable)
	void Server_ThrowWeapon(FVector Direction);
	UFUNCTION(NetMulticast, Reliable)
	void MC_ThrowWeapon(FVector Direction);

public:
	/**
	* @brief Called to drop the equiped weapon.
	*/
	virtual void DropWeapon();
protected:
	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void MC_DropWeapon();

	/**
	 * @brief Check if the player can pickup the weapon.
	 * @return true:Weapon can be picked up. false: weapon cannot be picked up
	 */
	bool CanPickup();

	/**
	* @brief Called when the weapon overlaps with another actor for pickup.
	* @param OverlappedComponent The component that overlaps.
	* @param OtherActor The actor that overlaps.
	* @param OtherComp The component of the other actor.
	* @param OtherBodyIndex The body index of the other actor.
	* @param bFromSweep Indicates if the overlap is from a sweep.
	* @param SweepResult The result of the sweep.
	*/
	UFUNCTION()
	virtual void OnOverlapPickupBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	* @brief Called when the weapon overlaps with another actor during a throw.
	* @param OverlappedComponent The component that overlaps.
	* @param OtherActor The actor that overlaps.
	* @param OtherComp The component of the other actor.
	* @param OtherBodyIndex The body index of the other actor.
	* @param bFromSweep Indicates if the overlap is from a sweep.
	* @param SweepResult The result of the sweep.
	*/
	UFUNCTION()
	virtual void OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	* @brief Triggers basic attack.
	* @param bButtonHeld if left click is helddown
	*/
	UFUNCTION()
	virtual void TriggerBasicAttack(bool bButtonHeld);

	/**
	* @brief Triggers special attack.
	* @param bButtonHeld if right click is helddown
	*/
	UFUNCTION()
	virtual void TriggerSpecialAttack(bool bButtonHeld);
	
	/**
	 * @brief Triggers an attack of the specified type.
	 * @param type The type of attack.
	 */
	UFUNCTION()
	virtual void TriggerAttackTimer(bool bButtonHeld);
	UFUNCTION(Server, Reliable)
	virtual void Server_TriggerAttackTimer(bool bButtonHeld);
	UFUNCTION(NetMulticast, Reliable)
	virtual void MC_TriggerAttackTimer(bool bButtonHeld);

	/**
	 * @brief Triggers an attack of the specified type.
	 * @param type The type of attack.
	 */
	UFUNCTION()
	virtual void TriggerSpecialAttackTimer(bool bButtonHeld);
	UFUNCTION(Server, Reliable)
	virtual void Server_TriggerSpecialAttackTimer(bool bButtonHeld);
	UFUNCTION(NetMulticast, Reliable)
	virtual void MC_TriggerSpecialAttackTimer(bool bButtonHeld);

	/**
	 * @brief Use this function to clear timers.
	 * @param Timer to be cleared.
	 */
	UFUNCTION()
	virtual void ClearAttackTimer(FTimerHandle timeHandle);

	/**
	 * @brief Applies damage to the specified actor.
	 * @param damagedActor The actor to damage.
	 * @param damage The amount of damage to apply.
	 * @param staggerDamage The amount of stagger damage to apply.
	 * @param applyInvulnerable apply Invulnerable to target.
	 * @param forceDead for the target to die-ah.
	 */
	virtual void ApplyDamage(IDamageInterface* damageReceiver);

	/**
	 * @brief Called when the player picks up the weapon.
	 * @param the player that picked up the weapon.
	 */
	virtual void OnPickup(AQuetzalMultiplayerCharacter* pickedUpActor);
	UFUNCTION(Server, Reliable)
	virtual void Server_OnPickup(AQuetzalMultiplayerCharacter* pickedUpActor);
	UFUNCTION(NetMulticast, Reliable)
	virtual void MC_OnPickup(AQuetzalMultiplayerCharacter* pickedUpActor);
	
	/**
	 * @brief Called when the player picks up the weapon.
	 * @param delta time used to substract pickup timer.
	 */
	void PickupWeaponTimerLogic(float deltaTime);

	virtual void SetupDefaultDamageInfo();

	// Inherited via ISpawnActorInterface
public:
	void SetRespawnTime(float time) override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_SetRespawnTime(float time);
	UFUNCTION(NetMulticast, Reliable)
	void MC_SetRespawnTime(float time);

public:
	void SetRespawnFlag(bool respawn) override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_SetRespawnFlag(bool respawn);
	UFUNCTION(NetMulticast, Reliable)
	void MC_SetRespawnFlag(bool respawn);

public:
	float GetRespawnTime() override;

	bool GetRespawnFlag() override;

	virtual void RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation) override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_RespawnActor(FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(NetMulticast, Reliable)
	void MC_RespawnActor(FVector spawnLocation, FRotator spawnRotation);
public:
	UFUNCTION()
	void StopAttackAnimation();
protected:
	UFUNCTION(Server, Reliable)
	void Server_StopAttackAnimation();
	UFUNCTION(NetMulticast, Reliable)
	void MC_StopAttackAnimation();
	/**************Functions**************************/
};
