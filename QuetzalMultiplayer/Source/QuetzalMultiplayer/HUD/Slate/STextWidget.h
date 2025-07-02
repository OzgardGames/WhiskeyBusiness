// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class QUETZALMULTIPLAYER_API STextWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STextWidget)
	{}
		SLATE_ARGUMENT(FText, ScoreNum)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void setText(FText text);
	void setText(FText text, class UObject* font, int fontSize);

protected:
	FText ScoreText;
	FSlateFontInfo ScoreFont;
	TSharedPtr<STextBlock> ScoreTextBlock;
};
