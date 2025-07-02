// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RecoveryAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API URecoveryAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	class AQuetzalMultiplayerCharacter* Character = nullptr;
	
};
