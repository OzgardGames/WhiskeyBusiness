// Fill out your copyright notice in the Description page of Project Settings.
/**
 * @file ACharacterController.h
 * @brief Controller class for the Character
 * @version 0.2
 * @date 2025-02-25
 *
 * 20250302-FB-v0.2: make character face cursor if mouse is used for attack actions
 * 
 * version 0.3
 * date: 2025-03-05
 * 
 * Description of Change: Added HUD creation in controller and toggle HUD functionality
 * 
 * Version 0.4
 * date: 2025-03-20
 * 
 * Description of Change: Updating Health with HP in characterState 
 * 
 * 
 * Version 0.5
 * date: 2025-04-08
 * 
 * Description of Change: Fixing bug with HP, multicasting set HP function
 * 
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedPlayerInput.h"
#include "DanceMinigameWidget.h"
#include "Player_GeometryCollectionF.h"
#include "CharacterController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class QUETZALMULTIPLAYER_API ACharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	ACharacterController();
	/***PLAYER INPUT***/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* inputMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* jumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* moveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* lookAction;

	/** Light Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* lightAttackAction;

	/** Throw Object Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* interactorThrow;

	/** Stun Character Input Action ** THIS IS A DEMO, WILL BE REMOVED WHEN CHARACTER CAN BE STUNNED */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StunAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** ToggleHUD Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleHUDAction;

	/***HUD INITIALIZATION***/

	/** HUD Template*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class AHUD> HUDTemplate;

	/** HUD **/
	UPROPERTY()
	class AQuetzalHUD* HUD;

	class ACharacterState* m_playerState;

	void CreateHUD();

	/** Special Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* specialAttackAction;

	/** Drop Weapon Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* dropWeaponAction;

	/********************/
	
	//GETTERS
	float GetMoveValue();
	//bool IsJumpPressed();
	bool IsInputGamepad();
	void MakeCharacterFaceMouseCursor(bool faceCursor);
	void onRespawn();

	// Red material for toggling effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* BaseMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	APlayer_GeometryCollectionF* BaseSpawnedActor;

	// Red material for toggling effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* RainBowMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	APlayer_GeometryCollectionF* RainBowSpawnedActor;

	// Original material for the bullet
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* GhostMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	APlayer_GeometryCollectionF* GhostSpawnedActor;

	// Red material for toggling effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* GoldMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")

	APlayer_GeometryCollectionF* GoldSpawnedActor;

	bool holdHitAudio = false;

protected:
	FTimerHandle inputCooldown;
	FTimerDelegate TimerDel;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnUnPossess() override;

	virtual void SetupInputComponent() override;

	virtual void Tick(float deltaTime) override;

	void UpdateHP(ACharacterState* characterState);

	UFUNCTION(Server, Reliable)
	void Server_UpdateHP(ACharacterState* characterState);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateHP(ACharacterState* characterState);


	/** Called for movement input */
	void OnMovePressed(const struct FInputActionValue& Value);

	/** Called for movement input stop*/
	void OnMoveReleased(const struct FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for jump input */
	void OnJumpPressed(const FInputActionValue& Value);

	/** Called for jump input stop */
	void OnJumpRelease(const FInputActionValue& Value);

	/** Called for Dash input */
	void DashTriggered(const FInputActionValue& Value);
	
	/** Called for Dash input -- DEMO FUNCTION */ 
	void StunTriggered(const FInputActionValue& Value);

	/** Called for Dance input -- DEMO FUNCTION */
	void DanceStunTriggered(const FInputActionValue& Value);

	/** Called for Attack input */
	void LightAttackPressed(const FInputActionValue& Value);

	void GrabSound();

	/** Called for Attack input */
	void LightAttackRelease(const FInputActionValue& Value);

	/** Called for Attack input */
	void SpecialAttackPressed(const FInputActionValue& Value);

	/** Called for Attack input */
	void SpecialAttackRelease(const FInputActionValue& Value);

	/** Called for Throwing held object input */
	void InteractorThrow(const FInputActionValue& Value);

	/** Called for HUD being toggling */
	void ToggleHUD(const FInputActionValue& Value);

	/** Called for Throwing held object input */
	void DropWeapon(const FInputActionValue& Value);

	class UEnhancedInputLocalPlayerSubsystem* inputSubsystem;
	
	class AQuetzalMultiplayerCharacter* character;

	/*******************
	
	  Dance Game Logic
	
	********************/
	void OnDanceGamePressed(FVector2D movementVector);
	void OnDanceGameRelease();
	TMap<EDanceArrowDirection, bool> arrowsinput;

	FTimerHandle HoldTimerHandle;
};
