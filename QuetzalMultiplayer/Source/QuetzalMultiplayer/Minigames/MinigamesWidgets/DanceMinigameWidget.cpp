// Fill out your copyright notice in the Description page of Project Settings.


#include "Minigames/MinigamesWidgets/DanceMinigameWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"

void UDanceMinigameWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Set the sets
    FiveArrowsSets.Add(     1   , { EDanceArrowDirection::A_LEFT, EDanceArrowDirection::A_LEFT, EDanceArrowDirection::A_UP,     EDanceArrowDirection::A_RIGHT,  EDanceArrowDirection::A_DOWN    });
    FiveArrowsSets.Add(     2   , { EDanceArrowDirection::A_LEFT, EDanceArrowDirection::A_LEFT, EDanceArrowDirection::A_RIGHT,  EDanceArrowDirection::A_UP,     EDanceArrowDirection::A_LEFT    });
    FiveArrowsSets.Add(     3   , { EDanceArrowDirection::A_LEFT, EDanceArrowDirection::A_UP,   EDanceArrowDirection::A_UP,     EDanceArrowDirection::A_DOWN,   EDanceArrowDirection::A_LEFT    });
    FiveArrowsSets.Add(     4   , { EDanceArrowDirection::A_LEFT, EDanceArrowDirection::A_UP,   EDanceArrowDirection::A_LEFT,   EDanceArrowDirection::A_DOWN,   EDanceArrowDirection::A_DOWN    });
    FiveArrowsSets.Add(     5   , { EDanceArrowDirection::A_DOWN, EDanceArrowDirection::A_DOWN, EDanceArrowDirection::A_UP,     EDanceArrowDirection::A_LEFT,   EDanceArrowDirection::A_RIGHT   });

    GenerateArrows();
}

void UDanceMinigameWidget::UpdateArrows(const TArray<EDanceArrowDirection>& currentSequence)
{
    arrowImages = { m_arrow1, m_arrow2, m_arrow3, m_arrow4, m_arrow5 };

    for (int32 i = 0; i < arrowImages.Num(); i++)
    {
        if (arrowImages[i] && currentSequence.IsValidIndex(i))
        {
            switch (i)
            {
            case 0:
                arrowImages[i] = Cast<UImage>(GetWidgetFromName("m_arrow1"));
                break;
            case 1:
                arrowImages[i] = Cast<UImage>(GetWidgetFromName("m_arrow2"));
                break;
            case 2:
                arrowImages[i] = Cast<UImage>(GetWidgetFromName("m_arrow3"));
                break;
            case 3:
                arrowImages[i] = Cast<UImage>(GetWidgetFromName("m_arrow4"));
                break;
            case 4:
                arrowImages[i] = Cast<UImage>(GetWidgetFromName("m_arrow5"));
                break;
            }
            RotateArrowImage(arrowImages[i], currentSequence[i]);
            SetArrowColor(arrowImages[i], false);
            arrowImages[i]->SetIsEnabled(true);
            arrowImages[i]->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void UDanceMinigameWidget::RotateArrowImage(UImage* arrowImage, const EDanceArrowDirection& arrowDirection)
{
    float RotationAngle = 0.0f;

    // Default arrow is facing down
    if (arrowDirection == EDanceArrowDirection::A_LEFT)
        RotationAngle = 90.0f;
    else if (arrowDirection == EDanceArrowDirection::A_UP)
        RotationAngle = 180.0f;
    else if (arrowDirection == EDanceArrowDirection::A_DOWN)
        RotationAngle = 0.0f;
    else if (arrowDirection == EDanceArrowDirection::A_RIGHT)
        RotationAngle = -90.0f;

    arrowImage->SetRenderTransformAngle(RotationAngle);
}

void UDanceMinigameWidget::SetArrowColor(UImage* arrowImage, bool isPressed)
{
    FSlateColor normalTint(FLinearColor(    0.8f,  0,  1,  1));
    FSlateColor pressedTint(FLinearColor(   1,     1,  1,  1));

    if (isPressed)
        arrowImage->SetBrushTintColor(pressedTint);
    else
        arrowImage->SetBrushTintColor(normalTint);
}

void UDanceMinigameWidget::GenerateArrows()
{
    playerWin = false;
    int32 RandomIndex;
    
    do
    {
        RandomIndex = FMath::RandRange(1, FiveArrowsSets.Num());
    } while (m_arrowsSet == FiveArrowsSets[RandomIndex]);

    m_arrowsSet = FiveArrowsSets[RandomIndex];

    m_currentIndex = 0;

    UpdateArrows(m_arrowsSet);
}

void UDanceMinigameWidget::CheckPlayerInput(EDanceArrowDirection InputKey)
{
    if (m_arrowsSet.IsValidIndex(m_currentIndex))
    {
        if (m_arrowsSet[m_currentIndex] == InputKey)
        {
            SetArrowColor(arrowImages[m_currentIndex], true);
            m_currentIndex++;
            if (m_currentIndex >= m_arrowsSet.Num())
            {
                playerWin = true;
            }
        }
        else
        {
            GenerateArrows();
        }
    }
}