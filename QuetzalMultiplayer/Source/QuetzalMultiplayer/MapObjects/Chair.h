// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PickupObjectsComp/PickupObjectInterface.h"
#include <Components/BoxComponent.h>
#include "Chair.generated.h"

UCLASS()
class QUETZALMULTIPLAYER_API AChair : public AActor, public IPickupObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChair();
	/** Skeletal mesh component for the weapon. */
	UPROPERTY(EditAnywhere, Category = Mesh)
	UStaticMeshComponent* mesh;
	/** Sphere collision component for pickup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThrowCollision")
	UBoxComponent* throwCollisionComp;
	// Inherited via IPickupObject
	void HighlightThisObject(bool set) override;

	void PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) override;
	UFUNCTION(Server, Reliable)
	void Server_PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) override;
	UFUNCTION(NetMulticast, Reliable)
	void MC_PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) override;
	void ThrowThisObject(FVector direction, float throwForce) override;
	UFUNCTION(Server, Reliable)
	void Server_ThrowThisObject(FVector direction, float throwForce) override;
	UFUNCTION(NetMulticast, Reliable)
	void MC_ThrowThisObject(FVector direction, float throwForce) override;
	void OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	void SetOwnerP(AActor* Actor) override;
	bool CanBePickedUp() override;
};
