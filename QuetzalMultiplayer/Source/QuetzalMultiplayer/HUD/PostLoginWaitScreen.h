// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PostLoginWaitScreen.generated.h"

/**
 * 
 */
UCLASS()
class QUETZALMULTIPLAYER_API UPostLoginWaitScreen : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* PlayButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* NumPlayersText = nullptr;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION()
	void PlayButtonClicked();

};
