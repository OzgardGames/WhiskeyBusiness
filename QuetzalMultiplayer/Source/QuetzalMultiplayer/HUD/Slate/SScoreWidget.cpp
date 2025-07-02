// Fill out your copyright notice in the Description page of Project Settings.


#include "SScoreWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SScoreWidget::Construct(const FArguments& InArgs)
{
	//This creates sets the brush that is established in the Blueprint for QuetzalHud
	ScoreBrush.SetResourceObject(InArgs._ScoreImage);
	ScoreBrush.ImageSize.X = InArgs._ScoreImage->GetSurfaceWidth();
	ScoreBrush.ImageSize.Y = InArgs._ScoreImage->GetSurfaceHeight();
	ScoreBrush.DrawAs = ESlateBrushDrawType::Image;


	//This creates new UI elements with the brush initialized above 
	//Most (if not all) of the animations will be done through QuetzalHUD.cpp where the HUD elements are created
	ChildSlot
		[
			SNew(SImage)
				.Image(&ScoreBrush)

			
		];

}



END_SLATE_FUNCTION_BUILD_OPTIMIZATION