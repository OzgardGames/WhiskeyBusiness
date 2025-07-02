// Copyright Epic Games, Inc. All Rights Reserved.
/**
 * @file QuetzalMultiplayerCharacter.h
 * @brief Player character class.
 * @version 0.8
 * @date 2025-01-20
 *
 *	20250129-FB-v0.2: added Melee attack
 *	20250202-FB-v0.3: added Ranged attack and throw action
 *	20250211-GH-v0.4: added BP Variables and linked them
 *	20250224-FB-v0.5: added Aiming
 * 	20250225-FB-v0.6: removed controller from character and put in a controller class
 * 	20250225-FB-v0.7: cleanup Tick function
 * 	20250302-FB-v0.8: added circle under character, improved logic for aiming.
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Weapon.h"
#include "CharacterState.h"
#include "CharacterController.h"
#include "CharacterAnimInstance.h"
#include "Player_GeometryCollectionF.h"
#include "SpawnBox.h"
#include <Components/BoxComponent.h>
#include "Components/PickupObjectsComp/PickupObjectInterface.h"
#include "Components/DamageSystemComp/DamageInterface.h"
#include "Components/DamageSystemComp/DamageSystemComp.h"
#include <WeaponInfoBar.h>
#include "QuetzalMultiplayerCharacter.generated.h"


 //TODO move to const or data assest file
const int PLAYER_HEALTH = 12;
const int PLAYER_STAGGER_HEALTH = 4;
const float PLAYER_STUN_DURATION_TIME = 3.f;
const float PLAYER_WALK_SPEED = 500.f;
const float PLAYER_AIM_WALK_SPEED = 200.f;
const float PLAYER_INVULNERABLE_VISIBILITY_DURATION = 0.05f;
const float PLAYER_INVULNERABLE_DURATION = 1.50f;
const float PLAYER_INPUT_HOLD_DURATION = 0.5f;
const float PLAYER_JUMP_Z_VELOCITY = 700.f;

class USpringArmComponent;
class UCameraComponent;
class UDanceMinigameWidget;
class AAudioManager;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * @class AQuetzalMultiplayerCharacter
 * @brief Player character class.
 */
UCLASS(config = Game)
class AQuetzalMultiplayerCharacter : public ACharacter, public IPickupObjectInterface, public IDamageInterface
{
	GENERATED_BODY()

	/********************/

	/***Animation***/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* standUpBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* standUpFrontMontage;
	
	/********************/



public:
	AQuetzalMultiplayerCharacter();

	//WaitingScreen
	UFUNCTION(BlueprintCallable)
	void CreateWaitingScreen();

	UPROPERTY()
	class UPostLoginWaitScreen* loginWidget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UUserWidget> loginWidgetTemplate;

	void SetPlayerStatePointer();

	/***Audio***/
	UPROPERTY(Replicated,EditAnywhere, Category = "Audio")
	TSubclassOf<AAudioManager> AudioManagerClass;

	UPROPERTY(Replicated)
	AAudioManager* AudioManager;


	/** Mesh for the circle under player. */
	UPROPERTY(BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshUnderCircle;
	/** Material for the circle under player. */
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterial* UnderCircleMaterial;

	// Red material for toggling effect
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* BaseMaterial;

	/** Dynamic Material for the circle. */
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInstanceDynamic* UnderCircleDynamicMaterialInst;

	/** Ragdoll Component attached to player. */
	UPROPERTY(EditAnywhere, Category = "Components")
	class URagdollComponent* RagdollComponent;

	/*************Damage System*****************/
	UDamageSystemComp* damageComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DEBUG_MSG_DamageResponse = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DEBUG_MSG_TakeDamage = false;
	//GETTERS
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual int GetHealth() override;
	virtual int GetMaxHealth() override;
	virtual int GetStaggerHealth() override;
	virtual int GetMaxStaggerHealth() override;
	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsInvulnerable() override;
	bool IsBlocking() override;
	//SETTERS
	void SetHealth(int setHealth) override;
	void SetStaggerHealth(int setStagger) override;
	void SetMaxHealth(int setMaxHealth) override;
	void SetMaxStaggerHealth(int setMaxStagger) override;
	UFUNCTION(BlueprintCallable, Category = "Invulnerable")
	void SetIsInvulnerable(bool setInvulnerable) override;
	void SetIsBlocking(bool setBlocking) override;
	virtual int Heal(int healAmount) override;

	virtual bool TakeDamage(FDamageInfo damageInfo) override;

	UFUNCTION(NetMulticast, Reliable)
	void MC_TakeDamage(FDamageInfo damageInfo);
	UFUNCTION(Server, Reliable)
	void Server_TakeDamage(FDamageInfo damageInfo);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void instaKill();
	UFUNCTION()
	void DamageResponceHandler(EDamageResponse damageResponse, FDamageInfo damageInfo);
	UFUNCTION()
	void DamageBlocked();
	/********************************************/

	UFUNCTION(BlueprintCallable, Category = "States")
	void ApplyStateChange(ECharacterState newState);
	UFUNCTION(Server, Reliable)
	void Server_ApplyStateChange(ECharacterState newState);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyStateChange(ECharacterState newState);

	UFUNCTION(BlueprintCallable, Category = "Die")
	void Die();

	void ChangeDeadMaterial();

	UFUNCTION(Server, Reliable)
	void Server_ChangeDeadMaterial();

	UFUNCTION(NetMulticast, Reliable)
	void MC_ChangeDeadMaterial();


	void DropWeaponAndObject(float value);
	UFUNCTION()
	void DropObject(float ObjectThrowForce=10.f);

	FVector deathFractureAngularVelocity;
	void GCImpulse();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Attack")
	void Server_GCImpulse();


	void HandleRespawnBoxAndMove();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Attack")
	void PlayerRespawn();

	//UFUNCTION(Server, Reliable)			//This didnt work to get rid of the boxes
	//void Server_HandleRespawnBoxAndMove();

	//UFUNCTION(NetMulticast, Reliable)			//This didnt work to get rid of the boxes
	//void Multicast_HandleRespawnBoxAndMove();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Respawn();


	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void Respawn();

	void ResetMovement();
	
	UPROPERTY(Replicated)
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayer_GeometryCollectionF> PlayerFracture;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASpawnBox> SpawnBoxes;

	//UPROPERTY(EditAnywhere)
	//TSubclassOf<AFieldSystemActor> MasterField;


	//void ResetAttackCooldown();
	/** Called for Attack input */
	void LightAttackRelease();

	/** Called for Attack input */
	void SpecialAttackPressed();

	/** Called for Attack input */
	void SpecialAttackRelease();

	UPROPERTY(Replicated)
	AWeapon* equipedWeapon;
	
	UFUNCTION()
	void StunState();
	void ClearStunState();
	UFUNCTION()
	void DanceStunState();
	void DanceClearStunState();

	void DashStart();
	void DashStop();

	//Temporal until find a way to replicate notify
	void AnimStop();

	void AimRotate(FRotator rot, bool face);

	UPROPERTY(BlueprintReadWrite, Replicated)
	FRotator aimRotation;

	UFUNCTION(Server, Reliable)
	void Server_SetRotation(FRotator rot, bool face);
	UFUNCTION(NetMulticast, Reliable)
	void MC_SetRotation(FRotator rot, bool face);

	UPROPERTY(BlueprintReadWrite, Replicated)
	UCharacterAnimInstance* myAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DEBUG_MSG_PLAYERSTATE = false;

	void SetOrientRotationToMovement(bool active);

	void setMovementSpeed(float speed);
	UFUNCTION(Server, Reliable)
	void Server_setMovementSpeed(float speed);
	UFUNCTION(NetMulticast, Reliable)
	void MC_setMovementSpeed(float speed);
	/*************Interact System*****************/
	/** Collision box of the area the player can interact with*/

	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* InteractRadius;
	/** Dynamic Material for the Highlight. */
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInstance* HighlightDynamicMaterialInst;
	/** List of all object that enter the InteractRadius overlap*/
	TArray<IPickupObjectInterface*> highLightedObject;
	/** Sphere collision component for pickup. */
	UPROPERTY(EditAnywhere, Category = "ThrowCollision")
	USphereComponent* throwCollisionComp;
	/** Name of the socket where to attach objects when held*/
	FName holdObjectSocketName;


	/** the character or object held by the player*/
	IPickupObjectInterface* objectBeingHeld;

	/** Is the player hold an object*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bIsHoldingObject;

	/** trigger overlap when an object enters the InteractRadius*/
	UFUNCTION()
	virtual void OnOverlapInteractBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/** trigger end overlap when an object leaves the InteractRadius*/
	UFUNCTION()
	virtual void OnOverlapInteractEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/** pickup highlighted object*/
	void PickupObject();
	UFUNCTION(Server, Reliable)
	void Server_ickupObject();
	UFUNCTION(NetMulticast, Reliable)
	void MC_PickupObject();


	/** Throw object or weapon held by the player*/
	void ThrowHeldObject();

	UFUNCTION(Server, Reliable)
	void Server_ThrowHeldObject();

	UFUNCTION(NetMulticast, Reliable)
	void MC_ThrowHeldObject();

	/** Inherited via PickupObjectInterface*/
	void SetOwnerP(AActor* Actor) override;
	virtual void HighlightThisObject(bool set) override;
	virtual void PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) override;
	UFUNCTION(Server, Reliable)
	void Server_PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) override;
	UFUNCTION(NetMulticast, Reliable)
	void MC_PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) override;
	virtual void ThrowThisObject(FVector direction, float throwForce = 500000) override;
	UFUNCTION(Server, Reliable)
	void Server_ThrowThisObject(FVector direction, float throwForce) override;
	UFUNCTION(NetMulticast, Reliable)
	void MC_ThrowThisObject(FVector direction, float throwForce) override;
	void OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	bool CanBePickedUp() override;
	/********************************************/

	float lightAttackInputHoldTimer;
	float specialAttackInputHoldTimer;

	UPROPERTY(BlueprintReadWrite, Replicated);
	bool lightAttackHoldSwitch;

	UPROPERTY(BlueprintReadWrite, Replicated);
	bool specialAttackHoldSwitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CharacterState)
	class ACharacterState* CharacterStateCPP;

	UFUNCTION()
	void OnRep_CharacterState();
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	APlayer_GeometryCollectionF* SpawnedActor;

	UPROPERTY(BlueprintReadWrite, Replicated)
	UGeometryCollectionComponent* GeometryCollectionComp;

	void SetRangeState(bool bNewRangeState);
	void SetMeleeSate(bool bNewMeleeState);

	UFUNCTION(Server, Reliable)
	void Server_SetPushState();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetPushState();

	UFUNCTION()
	void KnockBack();

	UFUNCTION(Server, Reliable)
	void Server_KnockBack(bool bNewKnockBackState);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_KnockBack(bool bNewKnockBackState);

	UFUNCTION(Server, Reliable)
	void Server_ThrowAnim();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ThrowAnim();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated, Category = "Combat")
	bool bCanAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Combat")
	bool bCanDash = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Combat")
	bool bCanGrab = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Combat")
	bool bCanRagdoll = true;

	void BlockAttackInput();
	void ResetAttackInput();

	void BlockDashInput();
	void ResetDashInput();

	void BlockGrabInput();
	void ResetGrabInput();

	void BlockRagdollInput();
	void ResetRagdollInput();

	UPROPERTY()
	UDanceMinigameWidget* DanceMinigameWidget;

	UPROPERTY()
	class UWinText* WinTextWidget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UUserWidget> WinTextWidgetClass;

	void checkForWinner();

	class ACharacterController* m_characterController;

	void SetCircleColor(const FVector& Color);

	UFUNCTION(Server, Reliable)
	void Server_SetCircleColor(const FVector& Color);

	UFUNCTION(NetMulticast, Reliable)
	void MC_SetCircleColor(const FVector& Color);


	// Original material for the bullet
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* OriginalMaterial;

	void SetMaterialS(UMaterialInterface* Material);

	UFUNCTION(Server, Reliable)
	void Server_SetMaterialS(UMaterialInterface* Material);

	UFUNCTION(NetMulticast, Reliable)
	void MC_SetMaterialS(UMaterialInterface* Material);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool isInlobbyCPP;

	void Lobby();

protected:

	float flashInvulnerableTimer;

	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	/**
	 * Called upon landing when falling, to perform actions based on the Hit result. Triggers the OnLanded event.
	 *  sets Character State IsOnGround to true;
	 * @param Hit Result describing the landing that resulted in a valid landing spot.
	 * @see OnMovementModeChanged()
	 */
	virtual void Landed(const FHitResult& Hit) override;

	/** Called when character's jump reaches Apex. Needs CharacterMovement->bNotifyApex = true */
	void NotifyJumpApex() override;

	//void CheckPlayerState(float deltaTime);

	void InvulnerableStateLogic(float deltaTime);

	void CheckIfLightAttackIsHeld(float deltaTime);

	void GrabAnimationStop();

	void ExitRagdollAfterTimer();





	/** TIMERS */
	FTimerHandle StunTimer;
	FTimerHandle DashTimer;
	FTimerHandle DashCooldownTimer;
	FTimerHandle GrabCheckTimer;
	FTimerHandle RagdollTimer;
	FTimerHandle AttackCooldownTimerHandle;
	FTimerHandle GrabCooldownTimerHandle;
	FTimerHandle KnockBackTimerHandle;
	FTimerHandle HoldPlayerTimerHandle;
	FTimerHandle RagdollCooldownTimerHandle;
	FTimerHandle PushCooldownTimerHandle;
	FTimerHandle ThrowCooldownTimerHandle;
	/** Timer Delegate for clearing attack timer. */
	FTimerDelegate HoldPlayerTimerDel;
	static constexpr float PLAYER_ATTACK_COOLDOWN = 1.0f;
	static constexpr float PLAYER_DASH_COOLDOWN = 2.0f;
	static constexpr float PLAYER_GRAB_COOLDOWN = 0.5f;
	static constexpr float PLAYER_HOLD_OTHER_PLAYER_DURATION = 3.f;
	static constexpr float PLAYER_RAGDOLL_COOLDOWN = 4.f;

	bool isRagdollStarted = false;
	bool isRespawning = false;
	/***********************
		  Dance Minigame
	************************/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSubclassOf<UUserWidget> DanceMinigameWidgetClass;

		FVector InstigatorLocation;

};

