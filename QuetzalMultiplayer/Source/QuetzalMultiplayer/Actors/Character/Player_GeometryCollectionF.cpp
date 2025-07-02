// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Character/Player_GeometryCollectionF.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"

// Sets default values
APlayer_GeometryCollectionF::APlayer_GeometryCollectionF()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create and attach a geometry collection component
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	RootComponent = GeometryCollectionComponent;

	// NO ACTIVAR FISICAS en el constructor
	GeometryCollectionComponent->SetSimulatePhysics(true); 
	GeometryCollectionComponent->SetNotifyRigidBodyCollision(true);
	GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollectionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	GeometryCollectionComponent->SetIsReplicated(true);

	GeometryCollectionComponent->SetVisibility(false, true); // Hide in Game
	GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//bReplicates = true;
	//SetReplicateMovement(true);
}

void APlayer_GeometryCollectionF::BeginPlay()
{
	Super::BeginPlay();
}

