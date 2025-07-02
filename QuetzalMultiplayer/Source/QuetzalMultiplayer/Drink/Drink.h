// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "QuetzalMultiplayerCharacter.h" 
#include "Drink.generated.h"

UENUM(BlueprintType)
enum class EDrinkType : uint8
{
	None         UMETA(DisplayName = "None"),
	HealingBrew  UMETA(DisplayName = "Healing"),
	NitroShot    UMETA(DisplayName = "Speed"),
	SpaceMilk    UMETA(DisplayName = "Highet"),
	TimeWarpWine UMETA(DisplayName = "TimeFlux")
};

UCLASS()
class QUETZALMULTIPLAYER_API ADrink : public AActor, public ISpawnActorInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrink();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Drink")
	virtual void ApplyEffect(AQuetzalMultiplayerCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Drink")
	virtual void ConsumeDrink();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Drink")
	virtual void FloatDrink(float DeltaTime);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink")
	FString DrinkName;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink")
	EDrinkType DrinkType;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drink")
	float EffectDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drink|Components")
	USphereComponent* CollisionSphere;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drink|Components")
	UStaticMeshComponent* DrinkMesh;


	float drinkFloatingTimer;
	FVector UpDownLocation;

public:
	void SetRespawnTime(float time) override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_SetRespawnTime(float time);
	UFUNCTION(NetMulticast, Reliable)
	void MC_SetRespawnTime(float time);

public:
	void SetRespawnFlag(bool respawn) override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_SetRespawnFlag(bool respawn);
	UFUNCTION(NetMulticast, Reliable)
	void MC_SetRespawnFlag(bool respawn);

public:
	float GetRespawnTime() override;
	bool GetRespawnFlag() override;
	void ActivateActor(bool active);

protected:
	UFUNCTION(Server, Reliable)
	void Server_ActivateActor(bool active);
	UFUNCTION(NetMulticast, Reliable)
	void MC_ActivateActor(bool active);
public:
	void RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation) override;
protected:
	UFUNCTION(Server, Reliable)
	void Server_RespawnActor(FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(NetMulticast, Reliable)
	void MC_RespawnActor(FVector spawnLocation, FRotator spawnRotation);
};
