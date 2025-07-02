// Copyright Epic Games, Inc. All Rights Reserved.
/****************************************************************************
*	Name: CharacterState
*	Version: 0.2
*	Created: 2025/01/20
*	Description:
*	- Charater posible states listted in an enum ECharacterState
*	- Timer for Invulnerable
*	- track if charater is on the ground
*	Change Log:
*	20250219-FB-v0.2: changed ACTIVE to WALKING and RUNNING
****************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CharacterState.generated.h"

UENUM(BlueprintType, Meta = (ScriptName = "ECharacterStateEnum"))
enum class ECharacterState : uint8
{
	IDLE			    UMETA(DisplayName = "IDLE"),
	RUNNING			    UMETA(DisplayName = "RUNNING"),
	DASHING			    UMETA(DisplayName = "DASHING"),
    KNOCKEDBACK		    UMETA(DisplayName = "KNOCKEDBACK"),
	RAGDOLLING		    UMETA(DisplayName = "RAGDOLLING"),
    RECOVERING		    UMETA(DisplayName = "RECOVERING"),
	STUNNED			    UMETA(DisplayName = "STUNNED"),
	DANCE_STUNNED	    UMETA(DisplayName = "DANCE_STUNNED"),
    DANCE_STUNNED_CLEAR UMETA(DisplayName = "DANCE_STUNNED_CLEAR"),
	ATTACKS			    UMETA(DisplayName = "ATTACKLS"),
	ATTACKR			    UMETA(DisplayName = "ATTACKLR"),
	SPECIALATTACKS	    UMETA(DisplayName = "SPECIAL_ATTACK_START"),
	SPECIALATTACKR	    UMETA(DisplayName = "SPECIAL_ATTACK_RELEASE"),
	JUMPING			    UMETA(DisplayName = "JUMPING"),
	//FALLING			    UMETA(DisplayName = "FALLING"),
	DEAD			    UMETA(DisplayName = "DEAD"),
	RESPAWNING		    UMETA(DisplayName = "RESPAWNING"),
	THROW			    UMETA(DisplayName = "THROW"),
	INTERACT		    UMETA(DisplayName = "INTERACT")
};

UCLASS()
class QUETZALMULTIPLAYER_API ACharacterState : public APlayerState
{
    GENERATED_BODY()
public:
    ACharacterState();

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "State")
    ECharacterState State = ECharacterState::IDLE;

    float InvulnerableTimer;
    
    class AQuetzalMultiplayerCharacter* characterPointer;

    UPROPERTY(Replicated)
    int PlayerIndex =0;

    UPROPERTY(Replicated)
    int HP = 0;
    UPROPERTY(Replicated, BlueprintReadWrite, Category = "State")
    int StockCPP = 3;

    bool needsRedraw = false;

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "State")
    bool IsOnGround;
};
