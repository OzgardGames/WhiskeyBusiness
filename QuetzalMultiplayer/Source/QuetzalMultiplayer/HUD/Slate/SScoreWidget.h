// Fill out your copyright notice in the Description page of Project Settings.

/****************************************************************************
*	Name: SScoreWidget
*	Version: 0.1
*	Created: 2025/02/13
*	Description:
*	- Slate Widget for the Score of each player
*	- All changes to score will need to be reflected here
*	Change Log:
****************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class QUETZALMULTIPLAYER_API SScoreWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SScoreWidget)
		:_ScoreImage(nullptr)
		{}
		SLATE_ARGUMENT(UTexture2D*, ScoreImage)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	//bool isPlayerScore;
protected:
	FSlateBrush ScoreBrush;

};
