
#include "TimeWarpWine.h"
#include "QuetzalMultiplayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ATimeWarpWine::ATimeWarpWine()
{
    PrimaryActorTick.bCanEverTick = true;

    DrinkName = TEXT("Time Warp Wine");
    DrinkType = EDrinkType::TimeWarpWine;  
    EffectDuration = 10.0f;  
    TimeDilationFactor = 2.0f; 
    OriginalMaxWalkSpeed = PLAYER_WALK_SPEED;
}

void ATimeWarpWine::ApplyEffect(AQuetzalMultiplayerCharacter* Character)
{
    if (!Character || !GetWorld())
    {
        return;
    }


    float OriginalGlobalTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;
    float OriginalCharacterTimeDilation = Character->CustomTimeDilation;

    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f);

    float OriginalSpeed = OriginalMaxWalkSpeed;
    Character->CustomTimeDilation = 2.0f;


  
    float RealEffectDuration = EffectDuration * 0.5f;

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle,
        [this, Character, OriginalSpeed,OriginalGlobalTimeDilation, OriginalCharacterTimeDilation]()
        {
            if (Character && !Character->IsPendingKill() && GetWorld())
            {
               
                UGameplayStatics::SetGlobalTimeDilation(GetWorld(), OriginalGlobalTimeDilation);
                Character->CustomTimeDilation = OriginalCharacterTimeDilation;
                UCharacterMovementComponent* CharMovement = Character->GetCharacterMovement();
                if (CharMovement)
                {
                    CharMovement->MaxWalkSpeed = OriginalSpeed;

                }
                
            }
        }, RealEffectDuration, false);


    Super::ApplyEffect(Character);

   
    //Destroy();
}