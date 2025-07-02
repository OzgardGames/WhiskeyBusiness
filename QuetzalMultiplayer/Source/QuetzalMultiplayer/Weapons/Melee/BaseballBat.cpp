// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseballBat.h"
#include "QuetzalMultiplayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/GameEngine.h"
#include "Net/UnrealNetwork.h"
ABaseballBat::ABaseballBat():
	AWeaponMelee(),
	chargeAttackTimer(0.f),
	maxChargeFlash(false),
	maxCharge(3.f)
{
	/*damageInfo.amountOfDamage = 1;
	damageInfo.amountOfStaggerDamage = 1;
	damageInfo.damageResponse = EDamageResponse::HIT_REACTION;
	damageInfo.damageType = ECustomDamageType::LIGHT_MELEE;
	damageInfo.DamageCauser = this;*/
	SetupDefaultDamageInfo();
	SetReplicates(true);
	SetReplicateMovement(true);

	chargehightlightColor = CreateDefaultSubobject<UMaterialInstance>(TEXT("InteractColor"));
	
	weaponRarity = EWeaponRarity::COMMON;
}

void ABaseballBat::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	chargeAttackColorLogic(deltaTime);
}

void ABaseballBat::TriggerBasicAttack(bool bButtonHeld)
{
	if(attachedActor)
	{
		if (!attachedActor->myAnimInstance->bIsMelee && !bButtonHeld)
		{
			SetupDefaultDamageInfo();
			/*damageInfo.amountOfDamage = 1;
			damageInfo.amountOfStaggerDamage = 1;
			damageInfo.damageResponse = EDamageResponse::HIT_REACTION;
			damageInfo.damageType = ECustomDamageType::LIGHT_MELEE;*/
			attachedActor->myAnimInstance->bIsMelee = true;
			GetWorld()->GetTimerManager().SetTimer(attackAnimationStopTimer, this, &AWeapon::StopAttackAnimation, 0.5f, false);
			attachedActor->myAnimInstance->bIsHoldMelee = false;
			Server_BasicAttack(bButtonHeld);
		}
		else if(bButtonHeld)
		{
			attachedActor->myAnimInstance->bIsHoldMelee = true;
			Server_BasicAttack(bButtonHeld);
		}
	}
}

void ABaseballBat::Server_BasicAttack_Implementation(bool bButtonHeld)
{
	MC_BasicAttack(bButtonHeld);
}

void ABaseballBat::MC_BasicAttack_Implementation(bool bButtonHeld)
{
	if(attachedActor != nullptr && attachedActor->myAnimInstance != nullptr)
	{
		if (!bButtonHeld)
		{
			attachedActor->myAnimInstance->bIsMelee = true;
			GetWorld()->GetTimerManager().SetTimer(attackAnimationStopTimer, this, &AWeapon::StopAttackAnimation, 0.5f, false);
			attachedActor->myAnimInstance->bIsHoldMelee = false;
		}
		else if (bButtonHeld)
		{
			attachedActor->myAnimInstance->bIsHoldMelee = true;
		}
	}
}

void ABaseballBat::TriggerSpecialAttack(bool bButtonHeld)
{
	if(attachedActor)
	{
		ACharacterController* controller = Cast<ACharacterController>(attachedActor->GetController());
		if (controller)
		{
			if (!bButtonHeld)
			{
				
				FString roleString = (HasAuthority()) ? "SERVER" : "CLIENT";

				damageInfo.damageType = ECustomDamageType::HEAVY_MELEE;
				damageInfo.damageResponse = EDamageResponse::KNOCK_BACK;
				damageInfo.impactForce = FVector(chargeAttackTimer * 1000.f);
				damageInfo.amountOfDamage = chargeAttackTimer;
				chargeAttackTimer = 0.f;
				controller->MakeCharacterFaceMouseCursor(false);
			}
			else
			{
				controller->MakeCharacterFaceMouseCursor(true);
			}
		}
	}
	Server_SpecialAttack(bButtonHeld);
}

void ABaseballBat::Server_SpecialAttack_Implementation(bool bButtonHeld)
{
	MC_SpecialAttack(bButtonHeld);
}

void ABaseballBat::MC_SpecialAttack_Implementation(bool bButtonHeld)
{
	if (attachedActor)
	{
		if (attachedActor->myAnimInstance) {
			attachedActor->myAnimInstance->bIsHoldMelee = bButtonHeld;
			if (!bButtonHeld)
			{
				skeletalMesh->SetOverlayMaterial(nullptr);
				skeletalMesh->SetMaterial(0, originalhightlightColor);
				attachedActor->myAnimInstance->bIsMelee = true;
				GetWorld()->GetTimerManager().SetTimer(attackAnimationStopTimer, this, &AWeapon::StopAttackAnimation, 0.5f, false);
				chargeAttackTimer = 0.f;
			}
		}
	}
}

void ABaseballBat::chargeAttackColorLogic(float deltaTime)
{
	if (attachedActor == nullptr)
	{
		return;
	}
	if (attachedActor->myAnimInstance==nullptr)
	{
		return;
	}

	if (attachedActor && attachedActor->myAnimInstance->bIsHoldMelee && bSpecialAttackButtonHeld)
	{
		chargeAttackTimer += deltaTime;
		chargeAttackTimer = FMath::Clamp(chargeAttackTimer, 0.f, maxCharge);

		
		UMaterialInstanceDynamic* chargehightlightMatInstance = skeletalMesh->CreateDynamicMaterialInstance(0, chargehightlightColor);
		if (chargehightlightMatInstance != nullptr)
		{
			if (chargeAttackTimer < maxCharge)//Charging up attack color
			{
				chargehightlightMatInstance->SetVectorParameterValue("Color", FLinearColor(chargeAttackTimer*.01f, 0.f, 0.f));
			}
			else //max charge
			{
				maxChargeFlash = !maxChargeFlash;
				if (maxChargeFlash)
				{
					chargehightlightMatInstance->SetVectorParameterValue("Color", FLinearColor(chargeAttackTimer, 0.f, 0.f));
				}
				else
				{
					chargehightlightMatInstance->SetVectorParameterValue("Color", FLinearColor(1.f, 1.f, 1.f));
				}
			}
			skeletalMesh->SetOverlayMaterial(chargehightlightMatInstance);
		}
	}
	else if (originalhightlightColor == nullptr)
	{
		originalhightlightColor = skeletalMesh->GetOverlayMaterial();
	}
}

void ABaseballBat::ThrowWeapon(FVector Direction)
{
	Server_ThrowBaseballBat(Direction);
}

void ABaseballBat::Server_ThrowBaseballBat_Implementation(FVector Direction)
{
	MC_ThrowBaseballBat(Direction);
}

void ABaseballBat::MC_ThrowBaseballBat_Implementation(FVector Direction)
{
	chargeAttackTimer = 0.f;
	bActivateWeaponCollisionCheck = false;
	skeletalMesh->SetOverlayMaterial(nullptr);
	skeletalMesh->SetMaterial(0, originalhightlightColor);
	Super::ThrowWeapon(Direction);
}

void ABaseballBat::DropWeapon()
{
	Server_DropBaseballBat();
}

void ABaseballBat::SetupDefaultDamageInfo()
{
	damageInfo.amountOfDamage = 1;
	damageInfo.amountOfStaggerDamage = 1;
	damageInfo.damageResponse = EDamageResponse::HIT_REACTION;
	damageInfo.damageType = ECustomDamageType::LIGHT_MELEE;
	damageInfo.DamageCauser = this;
}

void ABaseballBat::Server_DropBaseballBat_Implementation()
{
	MC_DropBaseballBat();
}

void ABaseballBat::MC_DropBaseballBat_Implementation()
{
	chargeAttackTimer = 0.f;
	bActivateWeaponCollisionCheck = false;
	skeletalMesh->SetOverlayMaterial(nullptr);
	skeletalMesh->SetMaterial(0, originalhightlightColor);
	Super::DropWeapon();
}