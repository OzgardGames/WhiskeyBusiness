// Fill out your copyright notice in the Description page of Project Settings.
/****************************************************************************
*	Name: MeleeCollisionCheckNotifyState
*	Version: 0.1
*	Created: 2025/01/20
*	Description:
*	Trigger this class to check if theres a melee 
*   collision during an animation
****************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeCollisionCheckNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API UMeleeCollisionCheckNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	class AQuetzalMultiplayerCharacter* Character = nullptr;
};
