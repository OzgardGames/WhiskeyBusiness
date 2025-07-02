// Fill out your copyright notice in the Description page of Project Settings.


#include "Chair.h"
#include <QuetzalMultiplayerCharacter.h>
#include "Engine/StaticMeshActor.h"
#include <Components/BoxComponent.h>
// Sets default values
AChair::AChair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*Mesh for Chair*/
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetAbsolute(false, false, true);
	RootComponent = mesh;
	/*Throwing collision*/
	throwCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ThrowCollision"));
	//throwCollisionComp->IgnoreActorWhenMoving(this, true);
	throwCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AChair::OnOverlapThrowBegin);
	throwCollisionComp->SetGenerateOverlapEvents(false);
	throwCollisionComp->SetupAttachment(RootComponent);
	/********************/

}

void AChair::HighlightThisObject(bool set)
{
	mesh->SetRenderCustomDepth(set);
}

void AChair::PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
	Server_PickupThisObject(charaterSkelMeshComp, socketName);
}

void AChair::Server_PickupThisObject_Implementation(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
	MC_PickupThisObject(charaterSkelMeshComp, socketName);
}

void AChair::MC_PickupThisObject_Implementation(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
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
		bIsPickedUp = true;
	}
}

void AChair::ThrowThisObject(FVector direction, float throwForce)
{
	Server_ThrowThisObject(direction, throwForce);
}

void AChair::Server_ThrowThisObject_Implementation(FVector direction, float throwForce)
{
	MC_ThrowThisObject(direction, throwForce);
}

void AChair::MC_ThrowThisObject_Implementation(FVector direction, float throwForce)
{
	if (bIsPickedUp)
	{
		mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		mesh->SetSimulatePhysics(true);
		mesh->SetNotifyRigidBodyCollision(true);
		mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		bIsPickedUp = false;
		mesh->AddImpulse(direction * (throwForce * 20));
		throwCollisionComp->SetGenerateOverlapEvents(true);
	}
}

void AChair::OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
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

		AStaticMeshActor* floorOrWall = Cast<AStaticMeshActor>(OtherActor);
		if(floorOrWall)
		{
			throwCollisionComp->SetGenerateOverlapEvents(false);//TODO needs more testing
		}
	}
}

void AChair::SetOwnerP(AActor* Actor)
{
	SetOwner(Actor);

}

bool AChair::CanBePickedUp()
{

	bCanBePickedUp = !bIsPickedUp;

	return bCanBePickedUp;
}