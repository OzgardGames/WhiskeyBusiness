// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowAnimNotify.h"
#include "QuetzalMultiplayerCharacter.h"
#include "CharacterAnimInstance.h"
#include "Engine.h"

void UThrowAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, __FUNCTION__);

	// check if the game is running and the world exists
	if (GetWorld()->IsGameWorld())
	{
		Character = Cast<AQuetzalMultiplayerCharacter>(MeshComp->GetOwner());
		
		//Character->ThrowHeldObject();

		if (Character->objectBeingHeld == nullptr)
		{
			return;
		}

		Character->objectBeingHeld->ThrowThisObject(Character->GetActorForwardVector(), 500.0f);

		Character->objectBeingHeld = nullptr;

	}
}
