// Fill out your copyright notice in the Description page of Project Settings.


#include "STextWidget.h"
#include "SlateOptMacros.h"
#include "Widgets/Text/STextBlock.h"
#include "Fonts/SlateFontInfo.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void STextWidget::Construct(const FArguments& InArgs)
{
	//Set the text to nothing so it doesnt appear at first
	//ScoreTextBlock->SetText(" ");
	
	ScoreText = InArgs._ScoreNum;
	
	FSlateFontInfo newFont = FSlateFontInfo("Arial", 50);

	//InArgs._ScoreNum.FromString(FString("WEEWOO"));
	ChildSlot
		[
			SAssignNew(ScoreTextBlock, STextBlock)
				.Text(InArgs._ScoreNum)
				.TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		];

}

void STextWidget::setText(FText text)
{
	ScoreText = text;
	ScoreTextBlock.ToSharedRef()->SetText(ScoreText);
}

void STextWidget::setText(FText text, UObject* font, int fontSize)
{
	FSlateFontInfo newFont = FSlateFontInfo(font, fontSize);
	ScoreText = text;
	ScoreTextBlock.ToSharedRef()->SetText(ScoreText);
	ScoreTextBlock.ToSharedRef()->SetFont(newFont);

}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION