/**
 * @file Weapon.cpp
 * @brief Implementation of the AWeapon class.
 * @version 0.2
 * @date 2025-02-02
 */

#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "QuetzalMultiplayerCharacter.h"
#include <Materials/MaterialInstanceConstant.h>
#include "Audio/AudioManager.h"

#include "Components/DamageSystemComp/DamageInterface.h"
#include <Net/UnrealNetwork.h>


AWeapon::AWeapon() :
	location(FVector(0.f)),
	weaponPickupTimer(0.f),
	flashPickUpTimer(0.f),
	bAttackButtonHeld(false),
	bSpecialAttackButtonHeld(false),
	UpDownLocation(FVector(0.f)),
	weaponFloatingTimer(0.f),
	isOneHandWeapon(true),
	isEquiped(false),
	attachedActor(nullptr),
	rightHandSocketName(FName("Hand_RSocket")),
	leftHandSocketName(FName("Hand_LSocket")),
	isRangedWeapon(false),
	weaponRarity(EWeaponRarity::NONE),
	forceSpawnWeapon(false),
	DEBUG_TRACE_WEAPON_MELEE(false)

{
	PrimaryActorTick.bCanEverTick = true;
	
	/*Pick up weapon collision (RootComponent)*/
	pickupCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	pickupCollisionComp->InitSphereRadius(30.0f);
	pickupCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapPickupBegin);
	pickupCollisionComp->SetGenerateOverlapEvents(true);
	RootComponent = pickupCollisionComp;
	/******************************************/

	/*Throwing collision*/
	throwCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("ThrowCollision"));
	throwCollisionComp->InitSphereRadius(30.0f);
	throwCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapThrowBegin);
	throwCollisionComp->SetGenerateOverlapEvents(false);
	throwCollisionComp->SetupAttachment(pickupCollisionComp);
	/********************/

	/*Skeletal Mesh for weapon*/
	skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	skeletalMesh->SetupAttachment(pickupCollisionComp);
	/**************************/
	SetReplicates(true);
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	//clear and setup Delegates for left and right click attacks
	this->OnAttack.Clear();
	this->OnSpecialAttack.Clear();
	this->OnAttack.AddDynamic(this, &AWeapon::TriggerAttackTimer);
	this->OnSpecialAttack.AddDynamic(this, &AWeapon::TriggerSpecialAttackTimer);
	ActivateActor(false);
	SetRespawnFlag(true);
	float weaponSpawnTimer = forceSpawnWeapon ? .1f : (float)GetWeaponRarity();
	SetRespawnTime(weaponSpawnTimer);

	

}

void AWeapon::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	WeaponFLoatingLogic(deltaTime);
	PickupWeaponTimerLogic(deltaTime);
}

/**
* Called when the weapon overlaps with another actor for pickup.
*/
void AWeapon::OnOverlapPickupBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if ((OtherActor != nullptr) && (OtherActor != this) && CanPickup())
	{
		AQuetzalMultiplayerCharacter* tempdActor = Cast<AQuetzalMultiplayerCharacter>(OtherActor);
		if (tempdActor && !isEquiped && isOneHandWeapon && !tempdActor->bIsHoldingObject)
		{
			OnPickup(tempdActor);
		}
	}
}
/**
* Called when the weapon overlaps with another actor during a throw.
*/
void AWeapon::OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		IDamageInterface* damageReceiver = Cast<IDamageInterface>(OtherActor);

		if (damageReceiver && damageInfo.InstigatedBy!= OtherActor)
		{
			ApplyDamage(damageReceiver);
		}
		SetRespawnFlag(true);
	}
}
/* called by the timer in TriggerAttackTimer, used in child classes*/
void AWeapon::TriggerBasicAttack(bool bButtonHeld) {}

/* called by the timer in TriggerSpecialAttackTimer, used in child classes*/
void AWeapon::TriggerSpecialAttack(bool bButtonHeld) {}

/* Called when left click it pressed or held*/
void AWeapon::TriggerAttackTimer(bool bButtonHeld)
{
	bAttackButtonHeld = bButtonHeld;
	Server_TriggerAttackTimer(bButtonHeld);
	auto& TimerManager = GetWorld()->GetTimerManager();
	float RemainingattackCooldown = 0.f;
	float RemainingspecialAttackCooldown = 0.f;
	if(TimerManager.TimerExists(attackCooldown))
		RemainingattackCooldown = TimerManager.GetTimerRemaining(attackCooldown);
	if (TimerManager.TimerExists(specialAttackCooldown))
		RemainingspecialAttackCooldown = TimerManager.GetTimerRemaining(specialAttackCooldown);
	//check if left click is held down and if the attack cooldown is active for the right click to prevent presseing both mouse buttonts at the same time
	if (bButtonHeld && RemainingspecialAttackCooldown <= 0 && !bSpecialAttackButtonHeld)
	{
		
		TriggerBasicAttack(bButtonHeld);
	}
	else if (RemainingattackCooldown <= 0.f && RemainingspecialAttackCooldown<=0 && !bSpecialAttackButtonHeld)
	{
		TriggerBasicAttack(bButtonHeld);
		TimerDel.BindUFunction(this, FName("ClearAttackTimer"), attackCooldown);
		TimerManager.SetTimer(attackCooldown, TimerDel, .7f, false);//.7 seems to prevent animation lock if spam attacking
	}
}

void AWeapon::Server_TriggerAttackTimer_Implementation(bool bButtonHeld)
{
	MC_TriggerAttackTimer(bButtonHeld);
}

void AWeapon::MC_TriggerAttackTimer_Implementation(bool bButtonHeld)
{
	bAttackButtonHeld = bButtonHeld;
}
/* Called when right click it pressed or held*/
void AWeapon::TriggerSpecialAttackTimer(bool bButtonHeld)
{
	bSpecialAttackButtonHeld = bButtonHeld;
	Server_TriggerSpecialAttackTimer(bButtonHeld);
	auto& TimerManager = GetWorld()->GetTimerManager();
	float RemainingattackCooldown = 0.f;
	float RemainingspecialAttackCooldown = 0.f;
	if (TimerManager.TimerExists(attackCooldown))
		RemainingattackCooldown = TimerManager.GetTimerRemaining(attackCooldown);
	if (TimerManager.TimerExists(specialAttackCooldown))
		RemainingspecialAttackCooldown = TimerManager.GetTimerRemaining(specialAttackCooldown);
	//check if right click is held down and if the attack cooldown is active for the left click to prevent presseing both mouse buttonts at the same time
	if (bButtonHeld && RemainingattackCooldown <= 0.f && !bAttackButtonHeld)
	{
		
		TriggerSpecialAttack(bButtonHeld);
	}
	else if (RemainingattackCooldown <= 0.f && RemainingspecialAttackCooldown <= 0 && !bAttackButtonHeld)
	{
		
		TriggerSpecialAttack(bButtonHeld);
		TimerDel.BindUFunction(this, FName("ClearAttackTimer"), specialAttackCooldown);
		TimerManager.SetTimer(specialAttackCooldown, TimerDel, .7f, false);//.7 seems to prevent animation lock if spam attacking
	}
}

void AWeapon::Server_TriggerSpecialAttackTimer_Implementation(bool bButtonHeld)
{
	MC_TriggerSpecialAttackTimer(bButtonHeld);
}

void AWeapon::MC_TriggerSpecialAttackTimer_Implementation(bool bButtonHeld)
{
	bSpecialAttackButtonHeld = bButtonHeld;
}

void AWeapon::ClearAttackTimer(FTimerHandle timeHandle)
{
	GetWorld()->GetTimerManager().ClearTimer(timeHandle);
}

/**
* Applies damage to anything that has the IDamageInterface
*/
void AWeapon::ApplyDamage(IDamageInterface* damageReceiver)
{
	if(damageReceiver)
	{
		damageInfo.DamageReceiver = damageReceiver;
		damageReceiver->TakeDamage(damageInfo);
	}
}

/**
* Throws the weapon in the specified direction.	
*/
void AWeapon::ThrowWeapon(FVector Direction)
{
	Server_ThrowWeapon(Direction);
}

void AWeapon::Server_ThrowWeapon_Implementation(FVector Direction)
{
	MC_ThrowWeapon(Direction);
}

void AWeapon::MC_ThrowWeapon_Implementation(FVector Direction)
{
	if (attachedActor == nullptr)
	{
		return;
	}
	if(attachedActor->myAnimInstance == nullptr)
	{
		return;
	}
	
	damageInfo.damageType = ECustomDamageType::THROW_WEAPON;
	damageInfo.amountOfDamage = 0;
	damageInfo.amountOfStaggerDamage = attachedActor->GetMaxStaggerHealth();
	damageInfo.DamageCauser = this;
	damageInfo.impactForce = FVector(500.0f, 0.0f, 0.0f);
	damageInfo.damageResponse = EDamageResponse::STUN;


	isEquiped = false;
	pickupCollisionComp->SetSimulatePhysics(true);
	pickupCollisionComp->SetNotifyRigidBodyCollision(true);
	pickupCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pickupCollisionComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	pickupCollisionComp->AddImpulse(Direction * 100000);
	throwCollisionComp->SetGenerateOverlapEvents(true);
	bAttackButtonHeld = false;
	bSpecialAttackButtonHeld = false;
	if (attachedActor != nullptr)
	{
		attachedActor->bIsHoldingObject = false;
		attachedActor->equipedWeapon = nullptr;
		if (attachedActor->myAnimInstance != nullptr)
		{
			attachedActor->myAnimInstance->bIsArmed = false;
			attachedActor->myAnimInstance->bIsAiming = false;
			attachedActor->myAnimInstance->bIsHoldMelee = false;
			attachedActor->myAnimInstance->bIsMelee = false;
			attachedActor->myAnimInstance->bIsRange = false;
		}
		attachedActor->setMovementSpeed(PLAYER_WALK_SPEED);
		attachedActor = nullptr;
	}
	SetOwner(nullptr);
}
/** Pick up weapon logic*/
void AWeapon::OnPickup(AQuetzalMultiplayerCharacter* pickedUpActor)
{
	Server_OnPickup(pickedUpActor);
}

void AWeapon::Server_OnPickup_Implementation(AQuetzalMultiplayerCharacter* pickedUpActor)
{
	MC_OnPickup(pickedUpActor);
}

void AWeapon::MC_OnPickup_Implementation(AQuetzalMultiplayerCharacter* pickedUpActor)
{
	if(pickedUpActor!=nullptr )
	{
		//TODO has athority
		attachedActor = pickedUpActor;
		damageInfo.InstigatedBy = attachedActor;
		isEquiped = true;
		attachedActor->bIsHoldingObject = true;
		attachedActor->equipedWeapon = this;
		SetOwner(attachedActor);
		SetInstigator(attachedActor);
		pickupCollisionComp->SetSimulatePhysics(false);
		pickupCollisionComp->SetGenerateOverlapEvents(false);
		pickupCollisionComp->AttachToComponent(attachedActor->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), rightHandSocketName);//TODO may need to change if we add two hand weapons
		if(attachedActor->myAnimInstance != nullptr)
		attachedActor->myAnimInstance->bIsArmed = true;
	}
}
/** check if weapon can be picked up*/
bool AWeapon::CanPickup()
{
	return weaponPickupTimer<=0.f;
}
/** Timer to flash weapon and indicate if when the weapon can be picked up*/
void AWeapon::PickupWeaponTimerLogic(float deltaTime)
{
	if (!CanPickup())
	{
		weaponPickupTimer -= deltaTime;

		if (weaponPickupTimer <= 0.0f)
		{
			weaponPickupTimer = 0.0f;

			pickupCollisionComp->SetGenerateOverlapEvents(true);

			skeletalMesh->SetVisibility(true);
		}
		else
		{
			flashPickUpTimer -= deltaTime;

			if (flashPickUpTimer <= 0.0f)
			{
				flashPickUpTimer = WEAPON_PICKUP_VISIBILITY_DURATION;

				skeletalMesh->ToggleVisibility();
			}
		}
	}
}

void AWeapon::SetupDefaultDamageInfo()
{
	
}

/** Drop weapon logic */
void AWeapon::DropWeapon()
{
	Server_DropWeapon();
}

void AWeapon::Server_DropWeapon_Implementation()
{
	MC_DropWeapon();
}

void AWeapon::MC_DropWeapon_Implementation()
{
	weaponPickupTimer = WEAPON_PICKUP_DELAY_DURATION;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	pickupCollisionComp->SetSimulatePhysics(false);
	pickupCollisionComp->SetNotifyRigidBodyCollision(true);
	pickupCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	isEquiped = false;
	bAttackButtonHeld = false;
	bSpecialAttackButtonHeld = false;
	if (attachedActor != nullptr)
	{
		attachedActor->bIsHoldingObject = false;
		attachedActor->equipedWeapon = nullptr;
		if (attachedActor->myAnimInstance != nullptr)
		{
			attachedActor->myAnimInstance->bIsArmed = false;
			attachedActor->myAnimInstance->bIsAiming = false;
			attachedActor->myAnimInstance->bIsHoldMelee = false;
			attachedActor->myAnimInstance->bIsMelee = false;
			attachedActor->myAnimInstance->bIsRange = false;
		}
		attachedActor->setMovementSpeed(PLAYER_WALK_SPEED);
		attachedActor = nullptr;
	}
	SetOwner(nullptr);
}

EWeaponRarity AWeapon::GetWeaponRarity()
{
	return weaponRarity;
}

void AWeapon::WeaponFLoatingLogic(float deltaTime)
{
	////Floating weapon logic
	if (!attachedActor)
	{
		UpDownLocation = GetActorLocation();
		FRotator spinRotation = FRotator(0.0f, 100.0f, 0.0f);

		weaponFloatingTimer += deltaTime;
		UpDownLocation.Z = GetActorLocation().Z + 0.2f * FMath::Sin(2 * PI * weaponFloatingTimer / 2);

		SetActorLocation(UpDownLocation);
		SetActorRotation(spinRotation * weaponFloatingTimer);
	}
}

void AWeapon::SetRespawnTime(float time)
{
	Server_SetRespawnTime(time);
}

void AWeapon::Server_SetRespawnTime_Implementation(float time)
{
	MC_SetRespawnTime(time);
}

void AWeapon::MC_SetRespawnTime_Implementation(float time)
{
	respawnTime = time;
}

void AWeapon::SetRespawnFlag(bool respawnFlag)
{
	Server_SetRespawnFlag(respawnFlag);
}

void AWeapon::Server_SetRespawnFlag_Implementation(bool respawn)
{
	MC_SetRespawnFlag(respawn);
}

void AWeapon::MC_SetRespawnFlag_Implementation(bool respawn)
{
	bRespawnFlag = respawn;
}

void AWeapon::ActivateActor(bool active)
{
	Server_ActivateActor(active);
}

void AWeapon::Server_ActivateActor_Implementation(bool active)
{
	MC_ActivateActor(active);
}

void AWeapon::MC_ActivateActor_Implementation(bool active)
{
	// Hides visible components
	SetActorHiddenInGame(!active);

	pickupCollisionComp->SetGenerateOverlapEvents(active);
	// Disables collision components
	//SetActorEnableCollision(active); //TODO this may turn on throw collision need testing

	// Stops the Actor from ticking
	SetActorTickEnabled(active);
}

float AWeapon::GetRespawnTime()
{
	return respawnTime;
}

bool AWeapon::GetRespawnFlag()
{
	return bRespawnFlag;
}

void AWeapon::RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation)
{
	Server_RespawnActor(spawnLocationAndRotation.spawnLocation, spawnLocationAndRotation.spawnRotation);
}

void AWeapon::Server_RespawnActor_Implementation(FVector spawnLocation, FRotator spawnRotation)
{
	MC_RespawnActor(spawnLocation, spawnRotation);
}

void AWeapon::MC_RespawnActor_Implementation(FVector spawnLocation, FRotator spawnRotation)
{
	SetupDefaultDamageInfo();
	ActivateActor(true);
	throwCollisionComp->SetGenerateOverlapEvents(false);
	DropWeapon();
	SetActorLocationAndRotation(spawnLocation, spawnRotation);
}

void AWeapon::StopAttackAnimation()
{
	Server_StopAttackAnimation();
}

void AWeapon::Server_StopAttackAnimation_Implementation()
{
	MC_StopAttackAnimation();
}

void AWeapon::MC_StopAttackAnimation_Implementation()
{
	if(attachedActor!=nullptr)
	{
		if (attachedActor->myAnimInstance != nullptr)
		{
			attachedActor->myAnimInstance->bIsMelee = false;
			attachedActor->myAnimInstance->bIsRange = false;
		}
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, damageInfo);
	DOREPLIFETIME(AWeapon, attachedActor);
	DOREPLIFETIME(AWeapon, weaponPickupTimer);
}