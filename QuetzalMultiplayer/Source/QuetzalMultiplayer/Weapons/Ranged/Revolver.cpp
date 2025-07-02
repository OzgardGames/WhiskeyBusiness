// Fill out your copyright notice in the Description page of Project Settings.


#include "Revolver.h"
#include "QuetzalMultiplayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Components/LineBatchComponent.h>
#include "Net/UnrealNetwork.h"
#include "ProjectileRevolver.h"
#include "Audio/AudioManager.h"
#include "GameFramework/CharacterMovementComponent.h"

ARevolver::ARevolver() :
	AWeaponRanged(),
	attackAngle(-45)
{
	PrimaryActorTick.bCanEverTick = true;

	currentAmmo = WEAPON_REVOLVER_MAX_AMMO;//FMath::RandRange(1, 6);//TODO needs gameplay testing to see how it feels

	ProjectileClass = AProjectileRevolver::StaticClass();
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultRevolverMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/Ranged/Revolver/Mesh/SM_pistol_001_SkelMesh.SM_pistol_001_SkelMesh'"));
	//skeletalMesh->SetSkeletalMesh(DefaultRevolverMesh.Object);

	SetupDefaultDamageInfo();

	weaponRarity = EWeaponRarity::EPIC;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ARevolver::BeginPlay()
{
	Super::BeginPlay();

	if (skeletalMesh && skeletalMesh->SkeletalMesh == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultRevolverMesh(
			TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/Ranged/Revolver/Mesh/SM_pistol_001_SkelMesh.SM_pistol_001_SkelMesh'")
		);

		if (DefaultRevolverMesh.Succeeded())
		{
			skeletalMesh->SetSkeletalMesh(DefaultRevolverMesh.Object);
		}
	}
}

void ARevolver::TriggerBasicAttack(bool bButtonHeld)
{
	if (attachedActor)
	{
		
		ACharacterController* controller = Cast<ACharacterController>(attachedActor->GetController());
		if (controller)
		{
			//if theres ammo and the button was released fire projectile
			if (!bButtonHeld)
			{
				Server_BasicAttack(bButtonHeld);
				//Stop the aim animation and play the fire ranged weapon animation
				if(currentAmmo > 0)
				{
					attachedActor->myAnimInstance->bIsAiming = false;
					attachedActor->myAnimInstance->bIsRange = true;
					GetWorld()->GetTimerManager().SetTimer(attackAnimationStopTimer, this, &AWeapon::StopAttackAnimation, 0.6f, false);
				}
				
				FVector projectileSpawnLocation = skeletalMesh->GetSocketLocation(muzzleSocketName);
				FRotator projectileSpawnRotation = attachedActor->GetActorRotation();

				//let the client with Authority fire the projectile
				if (HasAuthority() && currentAmmo > 0)
				{
					FActorSpawnParameters spawnParameters;
					spawnParameters.Instigator = GetInstigator();
					spawnParameters.Owner = this;
					AProjectileRevolver* spawnedProjectile = Cast<AProjectileRevolver>(GetWorld()->SpawnActor(ProjectileClass, &projectileSpawnLocation, &projectileSpawnRotation, spawnParameters));
					//prevent the projectile from hitting the character
					if (attachedActor->CharacterStateCPP->State == ECharacterState::RUNNING)
					{
						spawnedProjectile->ProjectileMovementComponent->InitialSpeed = 1500.0f + PLAYER_WALK_SPEED;
					}
				}
				currentAmmo--;
				controller->MakeCharacterFaceMouseCursor(false); //stop facing the mouse cursor once finished firring projectile
				attachedActor->myAnimInstance->bIsAiming = false;

			}
			else if (bButtonHeld)
			{
				Server_BasicAttack(bButtonHeld);
				//Face the Mouse Cursor and play the Aim animation.
				controller->MakeCharacterFaceMouseCursor(true);
				attachedActor->myAnimInstance->bIsAiming = true;
				attachedActor->myAnimInstance->bIsRange = false;
			}
		}
	}
	
}

void ARevolver::TriggerSpecialAttack(bool bButtonHeld)
{
	if (attachedActor)
	{
		ACharacterController* controller = Cast<ACharacterController>(attachedActor->GetController());
		if (controller != nullptr)
		{
			
			if (bButtonHeld == false)
			{
				currentRotation = attachedActor->GetActorRotation();
				originalRotation = currentRotation;

				TriggerBasicAttack(false);
				attackAngle = -45;

				//if we want the chacracter to turn while shootting
				/*auto& TimerManager = GetWorld()->GetTimerManager();
				TimerManager.SetTimer(revolverSpecialTimer, this, &ARevolver::RevolverSpecialAttack, .2f, true);*/

				//if we want the character to fire 3 infront right away
				for (int i = 0; i <= 3; i++)
				{
					RevolverSpecialAttack();
				}
				
				attachedActor->AimRotate(originalRotation, false);
				controller->MakeCharacterFaceMouseCursor(false);
				attachedActor->myAnimInstance->bIsAiming = false;
				Server_BasicAttack(bButtonHeld);
			}
			else if (bButtonHeld)
			{

				Server_BasicAttack(bButtonHeld);
				//Face the Mouse Cursor and play the Aim animation.
				controller->MakeCharacterFaceMouseCursor(true);
				attachedActor->myAnimInstance->bIsAiming = true;
				attachedActor->myAnimInstance->bIsRange = false;
			}
		}
	}
	
}
void ARevolver::RevolverSpecialAttack()
{
	if (attachedActor != nullptr)
	{
		if (currentAmmo % 3 == 0)
		{
			return;
		}
		if(attackAngle<=91 && attackAngle >= -46)
		{
			currentRotation.Yaw = currentRotation.Yaw + attackAngle;
			attackAngle *= -2;
		}
		else
		{
			attackAngle = -45;
			currentRotation = originalRotation;
		}

		attachedActor->AimRotate(currentRotation, true);
		TriggerBasicAttack(false);
	}
}



void ARevolver::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	IDamageInterface* damageReceiver = Cast<IDamageInterface>(OtherActor);
	if (damageReceiver)
	{
		//not needed because of damageInfo.damageResponse = EDamageResponse::INSTANT_KILL;
		damageInfo.amountOfDamage = 4;
		damageInfo.amountOfStaggerDamage = damageReceiver->GetMaxStaggerHealth();
		ApplyDamage(damageReceiver);
	}
}

/**
* Adjust Laser Distance off of the first target hit
*/
void ARevolver::AdjustLaserDistance()
{
	FVector start = skeletalMesh->GetSocketLocation(muzzleSocketName);
	FVector end = attachedActor->GetActorLocation() + (attachedActor->GetActorRotation().Vector() * 100.0f) + (attachedActor->GetActorForwardVector() * 100000.0f);
	FHitResult hitResult;
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(this);
	collisionQueryParams.AddIgnoredActor(pickupCollisionComp->GetAttachParentActor());

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionQueryParams);

	if (bHit)
	{
		float targetDistance = hitResult.Distance;
		FVector endLine = attachedActor->GetActorLocation() + (attachedActor->GetActorRotation().Vector() * 100.0f) + (attachedActor->GetActorForwardVector() * hitResult.Distance);
		//adjust laser point size to hit the first target	
		GetWorld()->LineBatcher->DrawLine(start, endLine, FLinearColor(1.0f, 0.0f, 0.0f, 0.1f), SDPG_World, 2, 100000.0);
	}
}

void ARevolver::SetupDefaultDamageInfo()
{
	damageInfo.DamageCauser = this;
	damageInfo.damageResponse = EDamageResponse::INSTANT_KILL;
	damageInfo.damageType = ECustomDamageType::HEAVY_RANGED;
	damageInfo.impactForce = FVector(500.0f, 0.0f, 0.0f);
}

void ARevolver::Server_BasicAttack_Implementation(bool bButtonHeld)
{
	MC_BasicAttack(bButtonHeld);
}

void ARevolver::MC_BasicAttack_Implementation(bool bButtonHeld)
{
	if (attachedActor != nullptr)
	{
		if (bButtonHeld)
		{
			if(attachedActor->myAnimInstance != nullptr)
			{
				//Face the Mouse Cursor and play the Aim animation.
				attachedActor->myAnimInstance->bIsAiming = true;
				attachedActor->myAnimInstance->bIsRange = false;
			}
			//slow down player speed
			UCharacterMovementComponent* MovementComp = attachedActor->GetCharacterMovement();
			if (MovementComp->GetMaxSpeed() == PLAYER_WALK_SPEED)
				attachedActor->setMovementSpeed(PLAYER_AIM_WALK_SPEED);
			//trigger laser pointer
			AdjustLaserDistance();
		}
		else
		{
			if (attachedActor->myAnimInstance != nullptr)
			{
				if(currentAmmo > 0)
				{
					//Stop the aim animation and play the fire ranged weapon animation
					attachedActor->myAnimInstance->bIsRange = true;
					GetWorld()->GetTimerManager().SetTimer(attackAnimationStopTimer,this, &AWeapon::StopAttackAnimation, 0.6f, false);
				}
				attachedActor->myAnimInstance->bIsAiming = false;
				
			}
			//set back the player speed
			attachedActor->setMovementSpeed(PLAYER_WALK_SPEED);
		}
	}

}

void ARevolver::RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation)
{
	Server_RespawnRevolver(spawnLocationAndRotation.spawnLocation, spawnLocationAndRotation.spawnRotation);
}
void ARevolver::Server_RespawnRevolver_Implementation(FVector spawnLocation, FRotator spawnRotation)
{
	MC_RespawnRevolver(spawnLocation, spawnRotation);
}

void ARevolver::MC_RespawnRevolver_Implementation(FVector spawnLocation, FRotator spawnRotation)
{
	
	currentAmmo = WEAPON_REVOLVER_MAX_AMMO;
	UStructSpawnLocationAndRotation spawnLocationAndRotation;
	spawnLocationAndRotation.spawnLocation = spawnLocation;
	spawnLocationAndRotation.spawnRotation = spawnRotation;
	Super::RespawnActor(spawnLocationAndRotation);
}