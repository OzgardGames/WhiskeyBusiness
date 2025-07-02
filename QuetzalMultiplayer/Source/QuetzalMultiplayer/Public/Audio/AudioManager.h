// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "AudioManager.generated.h"

UENUM(BlueprintType, Meta = (ScriptName = "EAudioEnum"))
enum class ECueAudio : uint8
{
	EMPTY				UMETA(DisplayName = "EMPTY"),
	BATHIT				UMETA(DisplayName = "BATHIT"),
	BATSLASH			UMETA(DisplayName = "BATSLASH"),
	MALLETHIT           UMETA(DisplayName = "MALLETHIT"),
	MALLETLASH			UMETA(DisplayName = "MALLETLASH"),
	OBJECTHIT			UMETA(DisplayName = "OBJECTHIT"),
	PUSHHIT			    UMETA(DisplayName = "PUSHHIT"),
	PUSHSLASH			UMETA(DisplayName = "PUSHSLASH"),
	DIE					UMETA(DisplayName = "DIE"),
	DAMAGE			    UMETA(DisplayName = "DAMAGE"),
	THROW			    UMETA(DisplayName = "THROW"),
	THROWHIT		    UMETA(DisplayName = "THROWHIT"),
	JUMP				UMETA(DisplayName = "JUMP"),
	SPECIALJUMP			UMETA(DisplayName = "SPECIALJUMP"),
	REVOLEREMPTY		UMETA(DisplayName = "REVOLEREMPTY"),
	REVOLERRELOAD		UMETA(DisplayName = "REVOLERRELOAD"),
	DANCESTUNNED		UMETA(DisplayName = "DANCESTUNNED"),
	STUNNED				UMETA(DisplayName = "STUNNED"),
	DASH				UMETA(DisplayName = "DASH"),
	LAUGH				UMETA(DisplayName = "LAUGH"),
	RICOCHET			UMETA(DisplayName = "RICOCHET"),
	PLAYERATTACK		UMETA(DisplayName = "PLAYERATTACK"),
	PLAYERGRAB			UMETA(DisplayName = "PLAYERGRAB"),
};



UCLASS()
class QUETZALMULTIPLAYER_API AAudioManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AAudioManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TMap<ECueAudio, USoundCue*> SoundCueMap;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayAudioCue(ECueAudio AudioType, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopAudioCue(ECueAudio AudioType);


	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetAudioCueVolume(ECueAudio AudioType, float volume);

private:
	UPROPERTY()
	TMap<ECueAudio, UAudioComponent*> ActiveAudioComponents;
};
