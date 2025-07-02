/**
 * @file WeaponMelee.cpp
 * @brief Implementation of the AWeaponMelee class.
 * @version 0.1
 * @date 2025-02-02
 */

#include "WeaponMelee.h"
#include "QuetzalMultiplayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"



AWeaponMelee::AWeaponMelee():
	AWeapon(), 
	bActivateWeaponCollisionCheck(false)
{
	isRangedWeapon = false;
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AWeaponMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bActivateWeaponCollisionCheck)
	{
		FHitResult hitActor = CheckMeleeCollision();
		FHitResult hitActor2 = CheckBetweenMeleeCollision();
		if (hitActor.GetActor() || hitActor2.GetActor())
		{
			if (attachedActor && (attachedActor != hitActor.GetActor() || attachedActor != hitActor2.GetActor()))
			{
				IDamageInterface* damageReceiver = Cast<IDamageInterface>(hitActor.GetActor());
				IDamageInterface* damageReceiver2 = Cast<IDamageInterface>(hitActor2.GetActor());
				if (damageReceiver)
				{
					ApplyDamage(damageReceiver);//apply damage to actor hit
				}
				else if (damageReceiver2)
				{
					ApplyDamage(damageReceiver2);//apply damage to actor hit
				}
			}

		}

	}
}

FHitResult AWeaponMelee::CheckMeleeCollision()
{
	FVector start;
	FVector end;
	FRotator rotation;
	skeletalMesh->GetSocketWorldLocationAndRotation("Start", start, rotation);
	skeletalMesh->GetSocketWorldLocationAndRotation("End", end, rotation);
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	actorsToIgnore.Add(attachedActor);
	EDrawDebugTrace::Type debugType = (DEBUG_TRACE_WEAPON_MELEE) ? EDrawDebugTrace::Type::ForDuration : EDrawDebugTrace::None;
	auto channel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		start,
		end,
		20.0f,
		channel,
		false,
		actorsToIgnore,
		debugType,
		hitResult,
		true
	);
	
	return hitResult;
}

//used to hit targets between fast animation frames
FHitResult AWeaponMelee::CheckBetweenMeleeCollision()
{
	FVector start;
	FVector end;
	FRotator rotation;
	skeletalMesh->GetSocketWorldLocationAndRotation("Hit_Start", start, rotation);
	skeletalMesh->GetSocketWorldLocationAndRotation("Hit_End", end, rotation);
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	actorsToIgnore.Add(attachedActor);
	EDrawDebugTrace::Type debugType = (DEBUG_TRACE_WEAPON_MELEE) ? EDrawDebugTrace::Type::ForDuration : EDrawDebugTrace::None;
	auto channel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		start,
		end,
		20.0f,
		channel,
		false,
		actorsToIgnore,
		debugType,
		hitResultTailEnd,
		true
	);

	return hitResultTailEnd;
}

void AWeaponMelee::Server_SetWeaponCollisionCheck_Implementation(bool bActive)
{
	Multicast_SetWeaponCollisionCheck(bActive);
}

void AWeaponMelee::Multicast_SetWeaponCollisionCheck_Implementation(bool bActive)
{
	bActivateWeaponCollisionCheck = bActive;
}



void AWeaponMelee::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponMelee, bActivateWeaponCollisionCheck);
}
