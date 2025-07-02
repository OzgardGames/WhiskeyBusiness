// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/WinText.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "CharacterState.h"
#include "GameFramework/GameState.h"
#include "Components/Button.h"

bool UWinText::Initialize()
{
    bool bResult = Super::Initialize();
    if (!bResult)
    {
        return false;
    }
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("WinText"));
    if (Widget != nullptr)
    {
        Widget->SetText(FText::FromString(" "));
    }

    if (MainMenu)
    {
        MainMenu->OnClicked.AddDynamic(this, &UWinText::MainMenuButtonClicked);
    }

    return true;
}

void UWinText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWinText::MainMenuButtonClicked()
{
    if (MainMenu)
    {

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC)
            {
                PC->ClientTravel("/Game/ThirPerson/Maps/UI_Maps/MainMenuMap.umap", TRAVEL_Absolute);
            }
        }
    }
}

void UWinText::DisplayWinnerText_Implementation()
{
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("WinText"));

    if (Widget != nullptr)
    {
        Widget->SetText(winner);
    }
}

