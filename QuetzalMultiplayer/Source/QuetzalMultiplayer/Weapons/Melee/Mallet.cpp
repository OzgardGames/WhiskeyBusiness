// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Melee/Mallet.h"
#include "DrawDebugHelpers.h"
#include "QuetzalMultiplayerCharacter.h"
#include "Engine/StaticMeshActor.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

AMallet::AMallet():AWeaponMelee(),
	spinSpeed(1000.f),
	spinAngle(0.f),
	spinDuration(3.f),
	spinCooldown(5.f),
	impulseFired(false)
{
	/*damageInfo.amountOfDamage = 5;
	damageInfo.amountOfStaggerDamage = 0;
	damageInfo.damageResponse = EDamageResponse::KNOCK_BACK;
	damageInfo.damageType = ECustomDamageType::HEAVY_MELEE;
	damageInfo.impactForce = FVector(1500.0f);
	damageInfo.DamageCauser = this;*/
	SetupDefaultDamageInfo();
	RadialForceComp = CreateDefaultSubobject<UCustomRadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComp->SetupAttachment(RootComponent);
	RadialForceComp->Radius = 300.f;
	RadialForceComp->ImpulseStrength = 1500;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->DEBUG_TRACE_RADIUS = DEBUG_TRACE_WEAPON_MELEE;
	RadialForceComp->actorsToIgnore.Add(this); 
	SetReplicates(true);
	SetReplicateMovement(true);

	spinCooldownHightlightColor = CreateDefaultSubobject<UMaterialInstance>(TEXT("cooldown Color"));

	weaponRarity = EWeaponRarity::RARE;
}

void AMallet::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (bActivateWeaponCollisionCheck)
	{
		checkFloorCollision();
		spinAngle += deltaTime * spinSpeed;

		auto& TimerManager = GetWorld()->GetTimerManager();
		float RemainingDurationTime = 0.f;
		if (TimerManager.TimerExists(spinAttackDuration))
			RemainingDurationTime = TimerManager.GetTimerRemaining(spinAttackDuration);

		if (RemainingDurationTime > 0.f)
		{
			spinAttackCooldownColorLogic();
		}
	}
	else
	{
		impulseFired = false;
		spinAngle = 0.f;
		auto& TimerManager = GetWorld()->GetTimerManager();
		float RemainingTime = 0.f;

		if (TimerManager.TimerExists(spinAttackCooldown))
			RemainingTime = TimerManager.GetTimerRemaining(spinAttackCooldown);

		if (RemainingTime > 0.f)
		{
			spinAttackCooldownColorLogic();
		}
	}
	if (originalHightlightColor == nullptr)
	{
		originalHightlightColor = skeletalMesh->GetOverlayMaterial();
	}

}

void AMallet::OnPickup(AQuetzalMultiplayerCharacter* pickedUpActor)
{
	Super::OnPickup(pickedUpActor);
	RadialForceComp->actorsToIgnore.AddUnique(pickedUpActor);
}

void AMallet::TriggerBasicAttack(bool bButtonHeld)
{
	if (attachedActor)
	{
		if (!bButtonHeld)
		{
			bActivateWeaponCollisionCheck = bButtonHeld;
			attachedActor->myAnimInstance->bIsMelee = true;
			GetWorld()->GetTimerManager().SetTimer(attackAnimationStopTimer, this, &AWeapon::StopAttackAnimation, 0.5f, false);
		}
	}
}
//Spin attack
void AMallet::TriggerSpecialAttack(bool bButtonHeld)
{
	if (attachedActor)
	{
		if (GetWorld()->GetTimerManager().GetTimerRemaining(spinAttackCooldown) <= 0.f)//check if spin attack is on cooldown
		{

			ACharacterController* controller = Cast<ACharacterController>(attachedActor->GetController());
			if (controller)
			{
				controller->MakeCharacterFaceMouseCursor(false);
			}
			if(attachedActor->myAnimInstance != nullptr)
			attachedActor->myAnimInstance->bIsHoldMelee = bButtonHeld;
			bActivateWeaponCollisionCheck = bButtonHeld;
			Server_Spin(bButtonHeld);

			if (bButtonHeld)//attack held
			{
				triggerSpinAttackDuration();
				ClearAttackTimer(spinAttackCooldown);
				attachedActor->AimRotate(FRotator(0.0f, -spinAngle, 0.0f), false);
			}
			else//attack released
			{
				triggerSpinAttackCooldown();
				ClearAttackTimer(spinAttackDuration);
				attachedActor->ApplyStateChange(ECharacterState::STUNNED);
			}
		}
	}
}

void AMallet::triggerSpinAttackDuration()
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	float RemainingTime = 0.f;
	if (TimerManager.TimerExists(spinAttackDuration))
		RemainingTime = TimerManager.GetTimerRemaining(spinAttackDuration);
	if (RemainingTime <= 0.f)
	{
		TimerManager.SetTimer(spinAttackDuration, this, &AMallet::triggerStopSpinAttack, spinDuration, false);
	}
}

void AMallet::triggerSpinAttackCooldown()
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	float RemainingTime = 0.f;
	if (TimerManager.TimerExists(spinAttackCooldown))
		RemainingTime = TimerManager.GetTimerRemaining(spinAttackCooldown);
	if (RemainingTime <= 0.f)
	{
		TimerManager.SetTimer(spinAttackCooldown, this, &AMallet::spinAttackCooldownColorLogic, spinCooldown, false);
	}
}

void AMallet::spinAttackCooldownColorLogic()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	float RemainingDurationTime = 0.f;
	if (TimerManager.TimerExists(spinAttackDuration))
		RemainingDurationTime = TimerManager.GetTimerRemaining(spinAttackDuration);
	float RemainingCooldownTime = 0.f;
	if (TimerManager.TimerExists(spinAttackCooldown))
		RemainingCooldownTime = TimerManager.GetTimerRemaining(spinAttackCooldown);

	if (RemainingDurationTime>0.f)
	{
		//start glowing red...
		setMalletColor(FLinearColor((spinDuration / RemainingDurationTime) / 200.f, 0.f, 0.f));
	}
	else if (RemainingCooldownTime > 0.f)
	{
		//fadding red...
		setMalletColor(FLinearColor(RemainingCooldownTime / 100.f, 0.f, 0.f));
	}
	else
	{
		//resetcolor
		setMalletColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	}
	
}

void AMallet::triggerStopSpinAttack()
{
	if (attachedActor)
	{
		attachedActor->ApplyStateChange(ECharacterState::SPECIALATTACKR);
	}
}

void AMallet::checkFloorCollision()
{
	//TODO check for new floor Mesh
	AStaticMeshActor* floorOrWall = nullptr;
	floorOrWall = Cast<AStaticMeshActor>(hitResult.GetActor());
	if (floorOrWall && !impulseFired)
	{
		RadialForceComp->FireImpulse();
		impulseFired = true;
	}
	else
	{
		floorOrWall = Cast<AStaticMeshActor>(hitResultTailEnd.GetActor());
		if (floorOrWall && !impulseFired)
		{
			RadialForceComp->FireImpulse();
			impulseFired = true;
		}
	}
}

void AMallet::setMalletColor(FLinearColor color)
{
	UMaterialInstanceDynamic* spinCooldownHightlightMatInstance = skeletalMesh->CreateDynamicMaterialInstance(0, spinCooldownHightlightColor);
	if (color.A>0.f && spinCooldownHightlightMatInstance != nullptr)
	{
		spinCooldownHightlightMatInstance->SetVectorParameterValue("Color", color);
		skeletalMesh->SetOverlayMaterial(spinCooldownHightlightMatInstance);
	}
	else
	{
		//resetcolor
		skeletalMesh->SetOverlayMaterial(nullptr);
		skeletalMesh->SetMaterial(0, originalHightlightColor);
	}
	Server_setMalletColor(color);
}

void AMallet::Server_setMalletColor_Implementation(FLinearColor color)
{
	MC_setMalletColor(color);
}

void AMallet::MC_setMalletColor_Implementation(FLinearColor color)
{
	UMaterialInstanceDynamic* spinCooldownHightlightMatInstance = skeletalMesh->CreateDynamicMaterialInstance(0, spinCooldownHightlightColor);
	if (color.A > 0.f && spinCooldownHightlightMatInstance != nullptr)
	{
		spinCooldownHightlightMatInstance->SetVectorParameterValue("Color", color);
		skeletalMesh->SetOverlayMaterial(spinCooldownHightlightMatInstance);
	}
	else
	{
		//resetcolor
		skeletalMesh->SetOverlayMaterial(nullptr);
		skeletalMesh->SetMaterial(0, originalHightlightColor);
	}
}

void AMallet::Server_Spin_Implementation(bool bButtonHeld)
{
	MC_Spin(bButtonHeld);
}

void AMallet::MC_Spin_Implementation(bool bButtonHeld)
{
	if (attachedActor)
	{
		if(attachedActor->myAnimInstance !=nullptr)
			attachedActor->myAnimInstance->bIsHoldMelee = bButtonHeld;
		bActivateWeaponCollisionCheck = bButtonHeld;
		if (bButtonHeld)
		{
			UCharacterMovementComponent* MovementComp = attachedActor->GetCharacterMovement();
			if (MovementComp->GetMaxSpeed() == PLAYER_WALK_SPEED)
				attachedActor->setMovementSpeed(PLAYER_AIM_WALK_SPEED);
		}
		else
		{
			attachedActor->setMovementSpeed(PLAYER_WALK_SPEED);
		}
	}
}

void AMallet::ThrowWeapon(FVector Direction)
{
	Server_ThrowMallet(Direction);
}

void AMallet::Server_ThrowMallet_Implementation(FVector Direction)
{
	MC_ThrowMallet(Direction);
}

void AMallet::MC_ThrowMallet_Implementation(FVector Direction)
{
	bActivateWeaponCollisionCheck = false;
	//resetcolor
	setMalletColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	if(attachedActor!=nullptr && GetWorld()!= nullptr)
	{
		float RemainingDurationTime = 0.f;
		auto& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.TimerExists(spinAttackDuration))
			RemainingDurationTime = TimerManager.GetTimerRemaining(spinAttackDuration);
		//stun player if they throw while spinning
		if (RemainingDurationTime > 0.f)
		{
			attachedActor->ApplyStateChange(ECharacterState::STUNNED);
		}
	}
	RadialForceComp->actorsToIgnore.Empty();
	ClearAttackTimer(spinAttackCooldown);
	ClearAttackTimer(spinAttackDuration);
	Super::ThrowWeapon(Direction);
}

void AMallet::DropWeapon()
{
	Server_DropMallet();
}

void AMallet::SetupDefaultDamageInfo()
{
	damageInfo.amountOfDamage = 5;
	damageInfo.amountOfStaggerDamage = 0;
	damageInfo.damageResponse = EDamageResponse::KNOCK_BACK;
	damageInfo.damageType = ECustomDamageType::HEAVY_MELEE;
	damageInfo.impactForce = FVector(1500.0f);
	damageInfo.DamageCauser = this;
}

void AMallet::Server_DropMallet_Implementation()
{
	MC_DropMallet();
}

void AMallet::MC_DropMallet_Implementation()
{
	bActivateWeaponCollisionCheck = false;
	//resetcolor
	setMalletColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	if (attachedActor != nullptr && GetWorld() != nullptr)
	{
		float RemainingDurationTime = 0.f;
		auto& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.TimerExists(spinAttackDuration))
			RemainingDurationTime = TimerManager.GetTimerRemaining(spinAttackDuration);
		//stun player if they throw while spinning
		if (RemainingDurationTime > 0.f)
		{
			attachedActor->ApplyStateChange(ECharacterState::STUNNED);
		}
	}
	RadialForceComp->actorsToIgnore.Empty();
	ClearAttackTimer(spinAttackCooldown);
	ClearAttackTimer(spinAttackDuration);
	Super::DropWeapon();
}