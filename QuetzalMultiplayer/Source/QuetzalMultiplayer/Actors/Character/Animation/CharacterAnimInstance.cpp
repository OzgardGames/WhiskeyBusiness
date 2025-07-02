// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "QuetzalMultiplayerCharacter.h"
#include "RagdollComponent.h"
#include "CharacterState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AQuetzalMultiplayerCharacter>(TryGetPawnOwner());
		
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character != nullptr)
	{
		bIsOnGround = !Character->GetCharacterMovement()->IsFalling();

		bIsMoving = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;

		bIsJumping = Character->GetCharacterMovement()->Velocity.Z > 0.0f;

		bIsFalling = Character->GetCharacterMovement()->Velocity.Z < 0.0f;

		//(Character->equipedWeapon) ? bIsArmed = true: bIsArmed = false;

		if (URagdollComponent* CharacterRagdoll = Character->FindComponentByClass<URagdollComponent>())
		{
			bIsRagdolling = CharacterRagdoll->bInRagdoll;
			bIsLayingOnBack = CharacterRagdoll->bIsLayingOnBack;
		}

	}
}

void UCharacterAnimInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterAnimInstance, bIsOnGround);
	DOREPLIFETIME(UCharacterAnimInstance, bIsMoving);
	DOREPLIFETIME(UCharacterAnimInstance, bIsJumping);
	DOREPLIFETIME(UCharacterAnimInstance, bIsFalling);
	DOREPLIFETIME(UCharacterAnimInstance, bIsDashing);
	DOREPLIFETIME(UCharacterAnimInstance, bIsPushing);
	DOREPLIFETIME(UCharacterAnimInstance, bIsArmed);
	DOREPLIFETIME(UCharacterAnimInstance, bIsMelee);
	DOREPLIFETIME(UCharacterAnimInstance, bIsRange);
	DOREPLIFETIME(UCharacterAnimInstance, bIsStunned);
	DOREPLIFETIME(UCharacterAnimInstance, bIsHoldMelee); 
	DOREPLIFETIME(UCharacterAnimInstance, bIsKnockedBack);
	DOREPLIFETIME(UCharacterAnimInstance, bIsAiming);
	DOREPLIFETIME(UCharacterAnimInstance, bIsRagdolling);
	DOREPLIFETIME(UCharacterAnimInstance, bIsLayingOnBack);
	DOREPLIFETIME(UCharacterAnimInstance, bIsGrabing);
	DOREPLIFETIME(UCharacterAnimInstance, bIsCarrying);
	DOREPLIFETIME(UCharacterAnimInstance, bIsThrowingObject);
	DOREPLIFETIME(UCharacterAnimInstance, bIsThrowingWeapon);



}



