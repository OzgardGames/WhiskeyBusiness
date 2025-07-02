// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "Components/DamageSystemComp/DamageInterface.h"
#include "Components/DamageSystemComp/DamageSystemComp.h"
#include <Components/BoxComponent.h>
#include <QuetzalMultiplayerCharacter.h>
#include "Sound/SoundCue.h"
#include "Piano.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API APiano : public AStaticMeshActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:
	const int PIANO_MAX_HEALTH = 3;
	/** Sphere collision component for pickup. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* collisionComp;

	UDamageSystemComp* damageComp;

	AQuetzalMultiplayerCharacter* instigator;

	/** cooldown for rightclick attack. */
	FTimerHandle pianoCooldown;

	// Original material for the bullet
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* originalMaterial0;
	// Original material for the bullet
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* originalMaterial1;
	// Original material for the bullet
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* originalMaterial2;

	// Red material for toggling effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* BrokenMaterial;

	/** Sound cue to play when the is restored. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundCue* RestoreSoundCue;

	/** Sound cue to play when the piano breaks completely. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundCue* BreakSoundCue;
	APiano();


	UFUNCTION()
	void OnBreakPiano();

	// Inherited via IDamageInterface
	int GetHealth() override;
	int GetMaxHealth() override;
	void SetMaxHealth(int setMaxHealth) override;
	UFUNCTION()
	void SetHealth(int setHealth) override;
	bool TakeDamage(FDamageInfo damageInfo) override;
	UFUNCTION()
	void SetIsDead(bool isDead);
	UFUNCTION()
	void SetPianoColor(UMaterialInterface* mat0, UMaterialInterface* mat1, UMaterialInterface* mat2);
	UFUNCTION()
	void resetPiano();
	//Not Used
	int GetStaggerHealth() override { return 0; };
	int GetMaxStaggerHealth() override { return 0; };
	bool IsInvulnerable() override { return false; };
	bool IsBlocking() override { return false; };
	void SetStaggerHealth(int setStagger) override {};
	void SetMaxStaggerHealth(int setMaxStagger) override {};
	void SetIsInvulnerable(bool setInvulnerable) override {};
	void SetIsBlocking(bool setBlocking) override {};
	int Heal(int healAmount) override { return 0; };
	
};
