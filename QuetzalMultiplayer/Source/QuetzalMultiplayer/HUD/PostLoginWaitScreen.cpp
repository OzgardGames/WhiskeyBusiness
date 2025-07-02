// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PostLoginWaitScreen.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "QuetzalMultiplayerGameMode.h"
#include "GameFramework/GameState.h"
#include "CharacterState.h"
#include "QuetzalMultiplayerCharacter.h"

void UPostLoginWaitScreen::NativeConstruct()
{
    Super::NativeConstruct();

    if (NumPlayersText)
    {
        NumPlayersText->SetText(FText::FromString(" "));
    }

  
    if (GetWorld()->GetFirstPlayerController()) {
        if (GetWorld()->GetFirstPlayerController()->HasAuthority())
        {
            if (PlayButton)
            {
                PlayButton->OnClicked.AddDynamic(this, &UPostLoginWaitScreen::PlayButtonClicked);
            }

        }
    }

}

void UPostLoginWaitScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    int numPlayers = 0;
    AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();
    if (GState && NumPlayersText)
    {
        numPlayers = GState->PlayerArray.Num();
        NumPlayersText->SetText(FText::FromString(FString::FromInt(numPlayers)));
    }
}

void UPostLoginWaitScreen::PlayButtonClicked()
{
    AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();

    for (auto pState : GState->PlayerArray)
    {
        ACharacterState* characterState = Cast<ACharacterState>(pState);
        if (characterState)
        {
            if (characterState->characterPointer)
                characterState->characterPointer->isInlobbyCPP = false;

        }
    }

    if (PlayButton)
    {
        PlayButton->SetIsEnabled(false);
        SetVisibility(ESlateVisibility::Hidden);
    }
}
