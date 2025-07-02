// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "QuetzalMultiplayerCharacter.h"
#include "WeaponRanged.h"
// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(7.5f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	//Registering the Projectile Impact function on a Hit event.
	
	RootComponent = SphereComponent;

	//Definition for the Mesh that will serve as our visual representation.
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(SphereComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 15000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	owernerOfProjectile = Cast<AWeaponRanged>(this->GetOwner());
	
	SetReplicates(true);
	SetReplicateMovement(true);
	bAlwaysRelevant = true;

}
void AProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}



