// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAnimNotify.h"
#include "QuetzalMultiplayerCharacter.h"
#include "CharacterAnimInstance.h"
#include "Engine.h"

void UMeleeAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, __FUNCTION__);

    //if (GetWorld()->IsGameWorld())
    //{
    //    Character = Cast<AQuetzalMultiplayerCharacter>(MeshComp->GetOwner());
    //    if (Character)
    //    {
    //        //Character->Server_SetMeleeState(false);
    //        Character->SetMeleeSate(false);
    //    }
    //}
}
