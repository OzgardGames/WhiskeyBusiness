// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeCollisionCheckNotifyState.h"
#include "QuetzalMultiplayerCharacter.h"
#include "WeaponMelee.h"

void UMeleeCollisionCheckNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Character = Cast<AQuetzalMultiplayerCharacter>(MeshComp->GetOwner());
    if (Character && Character->equipedWeapon && !Character->equipedWeapon->isRangedWeapon)
    {
        AWeaponMelee* MeleeWeapon = Cast<AWeaponMelee>(Character->equipedWeapon);
        {
            MeleeWeapon->Server_SetWeaponCollisionCheck(true);
        }
    }
}

void UMeleeCollisionCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Character = Cast<AQuetzalMultiplayerCharacter>(MeshComp->GetOwner());
    if (Character && Character->equipedWeapon && !Character->equipedWeapon->isRangedWeapon )
    {
        AWeaponMelee* MeleeWeapon = Cast<AWeaponMelee>(Character->equipedWeapon);
        {
            MeleeWeapon->Server_SetWeaponCollisionCheck(false);
        }
    }
}