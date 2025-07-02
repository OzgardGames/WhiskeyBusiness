// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockbackNotify.h"
#include "QuetzalMultiplayerCharacter.h"
#include "CharacterAnimInstance.h"
#include "RagdollComponent.h"
#include "Engine.h"

void UKnockbackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Yellow, __FUNCTION__);

	// Check if the game is running and the world exists
	if (GetWorld()->IsGameWorld())
	{
		Character = Cast<AQuetzalMultiplayerCharacter>(MeshComp->GetOwner());

		URagdollComponent* RagdollComponent = Character->FindComponentByClass<URagdollComponent>();

		if (RagdollComponent)
		{
			Character->Server_KnockBack(false);
		}
	}

}