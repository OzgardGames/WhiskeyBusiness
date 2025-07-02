// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RagdollComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUETZALMULTIPLAYER_API URagdollComponent : public UActorComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	URagdollComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable, BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Server_GoRagdoll();
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Multicast_GoRagdoll();
	void GoRagdoll();


	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ExitRagdoll();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void EnableMovementAtExit();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetCapsuleRotation();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void FindMeshDirection();

private:


	UPROPERTY(VisibleAnywhere)
	FName SocketName = FName("Body");

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CapsuleEndRotation = FRotator(0.0f, 0.0f, 0.0f);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void FollowRagdoll(float DeltaTime);


public:

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bInRagdoll = false;

	bool bIsLayingOnBack = false;

	FVector TargetGroundLocation;
	FVector BodyBaseOffset;
	FVector MeshLocation;
	FVector GroundOffset = FVector(0.0f,0.0f,20.0f);

};
