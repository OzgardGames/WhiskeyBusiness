// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthWidget.h"
#include "SlateOptMacros.h"
#include "Misc/Paths.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SHealthWidget::Construct(const FArguments& InArgs)
{
	//This creates sets the brush that is established in the Blueprint for QuetzalHud
	HeartBrush.SetResourceObject(InArgs._HeartImage);
	HeartBrush.ImageSize.X = InArgs._HeartImage->GetSurfaceWidth();
	HeartBrush.ImageSize.Y = InArgs._HeartImage->GetSurfaceHeight();
	HeartBrush.DrawAs = ESlateBrushDrawType::Image;

	
	//This creates new UI elements with the brush initialized above 
	//Most (if not all) of the animations will be done through QuetzalHUD.cpp where the HUD elements are created
	ChildSlot
	[
		SNew(SImage)
			.Image(&HeartBrush)
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
