// Fill out your copyright notice in the Description page of Project Settings.

/****************************************************************************
*	Name: SHealthWidget
*	Version: 0.1
*	Created: 2025/02/04
*	Description:
*	- Slate Widget for the Hearts of each player
*	- All changes to health will need to be reflected here
*	Change Log:
****************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class QUETZALMULTIPLAYER_API SHealthWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHealthWidget)
		:_HeartImage(nullptr)
	{}
		SLATE_ARGUMENT(UTexture2D*, HeartImage)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	FSlateBrush HeartBrush;
};
