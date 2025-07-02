// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MouseOverlay.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include <QuetzalMultiplayerCharacter.h>

bool UMouseOverlay::Initialize()
{
	bool bResult = Super::Initialize();
	if (!bResult)
	{
		return false;
	}

	return true;
}
//static int myValue = 0;

void UMouseOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	// We can include our changes to the widget text here
	AQuetzalMultiplayerCharacter* attachedActor= Cast<AQuetzalMultiplayerCharacter>(GetOwningPlayerPawn());
	if (attachedActor)
	{
		UCanvasPanel* mousePointer = Cast<UCanvasPanel>(GetWidgetFromName("Canvas_42"));
		mouseImage = Cast<UImage>(GetWidgetFromName("Image_42"));
		
	}

}
