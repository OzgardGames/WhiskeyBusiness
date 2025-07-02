// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:

	class AQuetzalMultiplayerCharacter* Character = nullptr;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsOnGround = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsJumping = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsDashing = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsPushing = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsArmed = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsHoldMelee = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsMelee = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsRange = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsStunned = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsKnockedBack = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsRagdolling = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsLayingOnBack = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsGrabing = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsCarrying = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsThrowingObject = false;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "AnimCharacter", meta = (AllowPrivateAccess = "true"))
	bool bIsThrowingWeapon = false;
};
