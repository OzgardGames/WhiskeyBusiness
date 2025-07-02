// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableObject.h"
#include <QuetzalMultiplayerCharacter.h>
#include "Engine/StaticMeshActor.h"
#include <Components/BoxComponent.h>
// Sets default values
APickableObject::APickableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*Mesh for Object*/
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetSimulatePhysics(true);
	mesh->SetAbsolute(false, false, true);
	RootComponent = mesh;
	/*Throwing collision*/
	throwCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ThrowCollision"));
	//throwCollisionComp->IgnoreActorWhenMoving(this, true);
	throwCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickableObject::OnOverlapThrowBegin);
	throwCollisionComp->SetGenerateOverlapEvents(false);
	throwCollisionComp->SetupAttachment(RootComponent);
	MassThrownCPP = 200;
	/********************/

	SetReplicates(true);
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
}

void APickableObject::HighlightThisObject(bool set)
{
	mesh->SetRenderCustomDepth(set);
}

void APickableObject::PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
	Server_PickupThisObject(charaterSkelMeshComp, socketName);
}

void APickableObject::Server_PickupThisObject_Implementation(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
	MC_PickupThisObject(charaterSkelMeshComp, socketName);
}

void APickableObject::MC_PickupThisObject_Implementation(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
	if (charaterSkelMeshComp == nullptr)
	{
		return;
	}
	if (!bIsPickedUp && CanBePickedUp())
	{
		//mesh->SetAbsolute(false, false, true);

		mesh->SetSimulatePhysics(false);
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mesh->AttachToComponent(charaterSkelMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), socketName);
		SetOwnerP(charaterSkelMeshComp->GetOwner());
		bIsPickedUp = true;
	}
}

void APickableObject::ThrowThisObject(FVector direction, float throwForce)
{
	Server_ThrowThisObject(direction, throwForce);
	
}

void APickableObject::StarOverlap()
{
	throwCollisionComp->SetGenerateOverlapEvents(true);
}

void APickableObject::OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		

	if ((OtherActor != nullptr)&& (OtherActor != this) && (OtherActor != GetOwner()))
	{
		
		SetOwnerP(nullptr);
		IDamageInterface* damageReceiver = Cast<IDamageInterface>(OtherActor);
		if (damageReceiver)
		{
			FDamageInfo damageInfo;
			damageInfo.amountOfStaggerDamage = damageReceiver->GetMaxStaggerHealth();
			damageInfo.InstigatedBy = attachedPlayer;
			damageInfo.DamageReceiver = damageReceiver;
			damageInfo.damageType = ECustomDamageType::IMPACT;
			damageInfo.DamageCauser = this;
			damageReceiver->TakeDamage(damageInfo);
		}
			throwCollisionComp->SetGenerateOverlapEvents(false);
	}
}

void APickableObject::SetOwnerP(AActor* Actor)
{
	Server_SetOwnerP(Actor);
	
}

void APickableObject::Server_SetOwnerP_Implementation(AActor* Actor)
{
	MC_SetOwnerP(Actor);
}

void APickableObject::MC_SetOwnerP_Implementation(AActor* Actor)
{
	SetOwner(Actor);
}

bool APickableObject::CanBePickedUp()
{

	bCanBePickedUp = !bIsPickedUp;

	return bCanBePickedUp;
}

void APickableObject::Server_ThrowThisObject_Implementation(FVector direction, float throwForce)
{
	MC_ThrowThisObject(direction, throwForce);
}

void APickableObject::MC_ThrowThisObject_Implementation(FVector direction, float throwForce)
{
	if (bIsPickedUp)
	{

		mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		mesh->SetSimulatePhysics(true);
		mesh->SetNotifyRigidBodyCollision(true);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bIsPickedUp = false;
		throwCollisionComp->SetGenerateOverlapEvents(true);

		//GetWorld()->GetTimerManager().SetTimer(DamageTimer, this, &APickableObject::StarOverlap, 0.1f);
		mesh->AddImpulse(direction * (throwForce * MassThrownCPP));

	}
}
