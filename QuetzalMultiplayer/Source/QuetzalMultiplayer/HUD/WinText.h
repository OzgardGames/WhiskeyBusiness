// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WinText.generated.h"

/**
 * 
 */

UCLASS()
class QUETZALMULTIPLAYER_API UWinText : public UUserWidget
{
	GENERATED_BODY()
	
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* MainMenu = nullptr;

	FText winner;

	UFUNCTION(NetMulticast, Reliable)
	void DisplayWinnerText();

	UFUNCTION()
	void MainMenuButtonClicked();

};
