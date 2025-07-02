// Fill out your copyright notice in the Description page of Project Settings.


#include "Drink/SpaceMilk/SpaceMilk.h"
#include "GameFramework/CharacterMovementComponent.h"

ASpaceMilk::ASpaceMilk()
{
    PrimaryActorTick.bCanEverTick = true;

	DrinkName = TEXT("Space Milk");
	DrinkType = EDrinkType::SpaceMilk;  
	EffectDuration = 15.0f;  
	JumpHeightMultiplier = 2.0f;  
    playerInitialJump = PLAYER_JUMP_Z_VELOCITY;
}

void ASpaceMilk::ApplyEffect(AQuetzalMultiplayerCharacter* Character)
{
    if (!Character)
    {
        return;
    }


    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {

        float OriginalJumpZ = playerInitialJump;

      
        MovementComp->JumpZVelocity *= JumpHeightMultiplier;

     
      

        
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Character, OriginalJumpZ]()
            {
                if (Character && !Character->IsPendingKill())
                {
                    UCharacterMovementComponent* CharMovement = Character->GetCharacterMovement();
                    if (CharMovement)
                    {
                        CharMovement->JumpZVelocity = OriginalJumpZ;
                       
                    }
                }
            }, EffectDuration, false);
    }


    Super::ApplyEffect(Character);

  
    //Destroy();
}
