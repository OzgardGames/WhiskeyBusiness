// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QUETZALMULTIPLAYER_API IPickupObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** is this object being held*/
	bool bIsPickedUp;
	bool bCanBePickedUp;
	AActor* attachedPlayer;
	/** Hightligh this object, pass nullprt to remove hightlight*/
	virtual void HighlightThisObject(bool set) = 0;
	/** call this function to attach to the skeletal mesh and socket name passed to the params*/
	virtual void PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)=0;
	UFUNCTION(Server, Reliable)
	virtual void Server_PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) = 0;
	UFUNCTION(NetMulticast, Reliable)
	virtual void MC_PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName) = 0;
	/** call this function throw the object in a direction and the amount of force to apply*/
	virtual void ThrowThisObject(FVector direction, float throwForce = 500000)=0;
	UFUNCTION(Server, Reliable)
	virtual void Server_ThrowThisObject(FVector direction, float throwForce)=0;
	UFUNCTION(NetMulticast, Reliable)
	virtual void MC_ThrowThisObject(FVector direction, float throwForce)=0;
	virtual void SetOwnerP(AActor* Actor) = 0;
	UFUNCTION()
	virtual void OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) =0;
	/** Create conditions if the object can be picked up*/
	virtual bool CanBePickedUp() = 0;



};
