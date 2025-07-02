// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player_GeometryCollectionF.generated.h"

UCLASS()
class QUETZALMULTIPLAYER_API APlayer_GeometryCollectionF : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayer_GeometryCollectionF();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGeometryCollectionComponent* GeometryCollectionComponent;

public:	

};
