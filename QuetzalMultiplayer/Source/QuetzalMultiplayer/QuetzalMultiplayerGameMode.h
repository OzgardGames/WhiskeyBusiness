// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QuetzalMultiplayerGameMode.generated.h"

UCLASS(minimalapi)
class AQuetzalMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
	TArray<class APlayerController*> characterPointersCPP;
	
	//FTimerHandle spawnTimer;

	TArray<AActor*> actorsToSpawn;

	AQuetzalMultiplayerGameMode();

	virtual void Tick(float deltaTime) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NumPlayers")
	int numPlayersCPP;

protected:


	// Max players allowed
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer")
	int32 MaxPlayers = 4;

	// Lock when game has started
	UPROPERTY(BlueprintReadWrite, Category = "Multiplayer")
	bool bGameStarted = false;

	void SpawnActorHandler(AActor* actor);
	UFUNCTION()
	void SpawnActor(AActor* actor);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool gameHasStarted = false;
};



