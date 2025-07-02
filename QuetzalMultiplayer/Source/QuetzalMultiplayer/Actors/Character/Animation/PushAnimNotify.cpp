// Fill out your copyright notice in the Description page of Project Settings.


#include "PushAnimNotify.h"
#include "QuetzalMultiplayerCharacter.h"
#include "CharacterAnimInstance.h"
#include "Engine.h"

void UPushAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    /*GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Green, __FUNCTION__);

    if (GetWorld()->IsGameWorld())
    {
        AQuetzalMultiplayerCharacter* MyCharacter = Cast<AQuetzalMultiplayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
            MyCharacter->Server_SetPushState(false);
    }*/
}
