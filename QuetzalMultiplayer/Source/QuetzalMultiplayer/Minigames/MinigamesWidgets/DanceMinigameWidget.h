// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DanceMinigameWidget.generated.h"

/**
 * 
 */
class UImage;
class UTexture2D;

UENUM(BlueprintType, Meta = (ScriptName = "EDanceArrowDirection"))
enum class EDanceArrowDirection : uint8
{
    A_LEFT			UMETA(DisplayName = "A_LEFT"),
    A_RIGHT			UMETA(DisplayName = "A_RIGHT"),
    A_UP			UMETA(DisplayName = "A_UP"),
    A_DOWN		    UMETA(DisplayName = "A_DOWN"),
    A_DEFAULT		UMETA(DisplayName = "A_DEFAULT"),
};

UCLASS()
class QUETZALMULTIPLAYER_API UDanceMinigameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateArrows(const TArray<EDanceArrowDirection>& currentSequence);

    // NOTES:
    //      Might use only one UImage pointer var later in the future.
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* m_arrow1 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* m_arrow2 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* m_arrow3 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* m_arrow4 = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* m_arrow5 = nullptr;

private:

    void RotateArrowImage(UImage* arrowImage, const EDanceArrowDirection& arrowDirection);
    void SetArrowColor(UImage* arrowImage, bool isPressed);

    TMap<int, TArray<EDanceArrowDirection>> FiveArrowsSets;

    int m_currentIndex = 0;
    TArray<EDanceArrowDirection> m_arrowsSet;
    TArray<UImage*> arrowImages;
    bool playerWin = true;
public:
    void GenerateArrows();
    void CheckPlayerInput(EDanceArrowDirection InputKey);
    bool GetHasWon() { return playerWin; }
};
