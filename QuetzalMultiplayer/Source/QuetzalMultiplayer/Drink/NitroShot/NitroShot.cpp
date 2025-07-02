
#include "NitroShot.h"
#include "GameFramework/CharacterMovementComponent.h"

ANitroShot::ANitroShot()
{
    PrimaryActorTick.bCanEverTick = true;

	DrinkName = TEXT("Nitro Shot");
	DrinkType = EDrinkType::NitroShot;
	EffectDuration = 10.0f; 
	SpeedMultiplier = 2.5f;  
    OriginalMaxWalkSpeed = PLAYER_WALK_SPEED;
	AffectedCharacter = nullptr;
}

void ANitroShot::ApplyEffect(AQuetzalMultiplayerCharacter* Character)
{
 if (!Character)
    {
        return;
    }
    
 
    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {
   
        float OriginalSpeed = OriginalMaxWalkSpeed;
        
      
        MovementComp->MaxWalkSpeed *= SpeedMultiplier;
     
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [Character, OriginalSpeed]()
        {
            if (Character && !Character->IsPendingKill())
            {
                UCharacterMovementComponent* CharMovement = Character->GetCharacterMovement();
                if (CharMovement)
                {
                    CharMovement->MaxWalkSpeed = OriginalSpeed;
                    
                }
            }
        }, EffectDuration, false);
    }
    
   
    Super::ApplyEffect(Character);
    
    
    //Destroy();
}