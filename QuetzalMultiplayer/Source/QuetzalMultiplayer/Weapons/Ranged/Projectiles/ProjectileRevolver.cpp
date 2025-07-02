// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRevolver.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "QuetzalMultiplayerCharacter.h"
#include "Revolver.h"
#include "Projectile.h"
#include "Public/Audio/AudioManager.h"
// Sets default values
AProjectileRevolver::AProjectileRevolver() :
    AProjectile(),
    maxNumberOfBounces(3),
    hitIgnoredActor(false)
{
    PrimaryActorTick.bCanEverTick = true;


    SphereComponent->OnComponentHit.AddDynamic(this, &AProjectileRevolver::OnProjectileImpact);
    ProjectileMovementComponent->bShouldBounce = true;
    ProjectileMovementComponent->Bounciness = 1.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;

    SetReplicates(true);
    SetReplicateMovement(true);
    bAlwaysRelevant = true;
}


void AProjectileRevolver::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AActor* ignoreActor = Cast<ARevolver>(this->GetOwner())->attachedActor;//TODO move this to on spawn or something...
    owernerOfProjectile = Cast<ARevolver>(this->GetOwner());//TODO move this to on spawn or something...
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != this->GetOwner()))
    {
        IDamageInterface* damageReceiver = Cast<IDamageInterface>(OtherActor);
        if (damageReceiver /*&& owernerOfProjectile*/)
        {
            if (OtherActor != ignoreActor || hitIgnoredActor)
            {
                owernerOfProjectile->OnProjectileHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
            }
        }
    }

    

    AQuetzalMultiplayerCharacter* character = Cast<AQuetzalMultiplayerCharacter>(ignoreActor);
    if (character)
    {
        if (character->AudioManager != nullptr)
            character->AudioManager->PlayAudioCue(ECueAudio::RICOCHET, character->GetActorLocation());
    }

    maxNumberOfBounces -= 1;
    if (maxNumberOfBounces <= 0)
    {
        Destroy();
    }
    else
    {
        hitIgnoredActor = true;
    }
}

