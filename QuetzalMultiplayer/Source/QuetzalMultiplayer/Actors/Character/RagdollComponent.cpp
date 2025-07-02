// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Character/RagdollComponent.h"
#include "Character/QuetzalMultiplayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
URagdollComponent::URagdollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void URagdollComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh())
		{
			BodyBaseOffset = CharacterMesh->GetRelativeLocation();
		}
	}

}

// Called every frame
void URagdollComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FollowRagdoll(DeltaTime);
}

void URagdollComponent::Server_GoRagdoll_Implementation()
{
	Multicast_GoRagdoll();
}

//void URagdollComponent::Multicast_GoRagdoll_Implementation()
//{
//	// Set state variable
//	bInRagdoll = true;
//
//	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
//	{
//		// Change the collision type and enable Query and Physics on the Mesh
//		if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh())
//		{
//
//			// Disable movement in the CharacterMovement component
//			if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
//			{
//				// Disable collision for the capsule component
//				if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
//				{
//
//					// Enable collisions and physics
//					// Activate physics simulation below a specific bone (Body in this case)
//					CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//					CharacterMesh->SetAllBodiesBelowSimulatePhysics(FName("BodyBase"), true, true);
//
//					//Disable Movement During Ragdoll
//					//CharacterMovement->SetMovementMode(EMovementMode::MOVE_None);
//
//					Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
//				}
//			}
//		}
//	}
//}

void URagdollComponent::Multicast_GoRagdoll_Implementation()
{
	// Set state variable
	bInRagdoll = true;

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh())
		{
			if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
			{
				if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
				{
					// Enable collision and physics simulation on the skeletal mesh
					CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					CharacterMesh->SetAllBodiesBelowSimulatePhysics(FName("BodyBase"), true, true);

					// Set collision profile to "Ragdoll" for correct physics interaction
					CharacterMesh->SetCollisionProfileName(TEXT("Ragdoll"));

					// Enable Continuous Collision Detection (CCD) to prevent tunneling through objects
					CharacterMesh->BodyInstance.bUseCCD = true;
					CharacterMesh->SetAllUseCCD(true);

					// Clamp linear velocity to prevent extreme speeds from breaking physics
					FVector CurrentVelocity = CharacterMesh->GetPhysicsLinearVelocity();
					CharacterMesh->SetAllPhysicsLinearVelocity(CurrentVelocity.GetClampedToMaxSize(1000.0f));

					// Set capsule collision to Query Only during ragdoll state
					Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				}

				//  disable character movement completely during ragdoll
				 CharacterMovement->SetMovementMode(EMovementMode::MOVE_None);
			}
		}
	}
}

// Activates Ragdoll state
void URagdollComponent::GoRagdoll()
{
	Server_GoRagdoll();
}

void URagdollComponent::ExitRagdoll()
{
	if (!bInRagdoll) { return; }
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		// Change the collision type and enable Query and Physics on the Mesh
		if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh())
		{
			// Disable movement in the CharacterMovement component
			if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
			{
				FindMeshDirection();
				
				CharacterMesh->GetAnimInstance()->SavePoseSnapshot("RagdollLastPose");
				CharacterMesh->BodyInstance.bUseCCD = false;
				CharacterMesh->SetAllUseCCD(false);
				SetCapsuleRotation();

				// Set state variable
				bInRagdoll = false;
				
				// Disable collision for the capsule component
				if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
				{
					CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					CharacterMesh->SetAllBodiesSimulatePhysics(false);
					CharacterMesh->bUpdateJointsFromAnimation;

					Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				}
			}
		}
	}
}

void URagdollComponent::EnableMovementAtExit()
{
	if (AQuetzalMultiplayerCharacter* Character = Cast<AQuetzalMultiplayerCharacter>(GetOwner()))
	{
		// Disable movement in the CharacterMovement component
		if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
		{
			Character->ClearStunState();
			Character->ApplyStateChange(ECharacterState::IDLE);
			CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
			CharacterMovement->bOrientRotationToMovement = true;
		}
	}
}

void URagdollComponent::SetCapsuleRotation()
{
	if (USkeletalMeshComponent* CharacterMesh = Cast<ACharacter>(GetOwner())->GetMesh())
	{
		FVector HeadLocation = CharacterMesh->GetSocketLocation("Head");
		FVector BodyLocation = CharacterMesh->GetSocketLocation("BodyBase");

		FVector BodyDirection = HeadLocation - BodyLocation;

		if (bIsLayingOnBack)
		{
			BodyDirection *= -1;
		}

		FRotator CapsuleRotation = UKismetMathLibrary::MakeRotFromZX(FVector(0.0f, 0.0f, 1.0f), BodyDirection );
		
		if (UCapsuleComponent* Capsule = Cast<ACharacter>(GetOwner())->GetCapsuleComponent())
		{
			Capsule->SetWorldRotation(CapsuleRotation);
		}
	}
}

void URagdollComponent::FindMeshDirection()
{
	if (USkeletalMeshComponent* CharacterMesh = Cast<ACharacter>(GetOwner())->GetMesh())
	{
		FRotator BodyRotation = CharacterMesh->GetSocketRotation("Body");
		FVector Vector = UKismetMathLibrary::GetForwardVector(BodyRotation);

		if (Vector.Z > 0)
		{
			bIsLayingOnBack = true;
		}
		else
		{
			bIsLayingOnBack = false;
		}
	}
}

void URagdollComponent::FollowRagdoll(float DeltaTime)
{
	if (!bInRagdoll) { return; }
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		// Change the collision type and enable Query and Physics on the Mesh
		if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh())
		{
			// Disable collision for the capsule component
			if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Character);

				FVector BodyBaseLocation = CharacterMesh->GetSocketLocation("BodyBase");

				FVector TraceDirection;
				TraceDirection = CharacterMesh->GetSocketLocation("BodyBase") + FVector(0.0f,0.0f,-100.0f);

				bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,
					BodyBaseLocation,
					TraceDirection,
					ECollisionChannel::ECC_Visibility,
					Params);

				if (bHit)
				{
					//DrawDebugLine(GetWorld(), BodyBaseLocation, HitResult.Location, FColor::Green, false, 0.1f);
					TargetGroundLocation = HitResult.Location;
				}
				else
				{
					//DrawDebugLine(GetWorld(), BodyBaseLocation, TraceDirection, FColor::Red, false, 0.1f);
					TargetGroundLocation = BodyBaseLocation;
				}

				MeshLocation = TargetGroundLocation - BodyBaseOffset;

				Capsule->SetWorldLocation(MeshLocation);
			}
		}
	}
}

void URagdollComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URagdollComponent, bInRagdoll);

}
