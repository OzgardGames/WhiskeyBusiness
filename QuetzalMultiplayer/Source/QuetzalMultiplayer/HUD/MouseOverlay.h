// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MouseOverlay.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API UMouseOverlay : public UUserWidget
{

	GENERATED_BODY()

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UImage* mouseImage;
};
