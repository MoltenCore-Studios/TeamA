// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "ForgingWidget.h"

void UForgingWidget::UpdateForgePrompt(const FString& NewText)
{
	if (ForgePrompt)
	{
		ForgePrompt->SetText(FText::FromString(NewText));
	}
}

void UForgingWidget::ShowForgePrompt(bool bShow)
{
	if (ForgePrompt)
	{
		ForgePrompt->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::ShowCrosshair(bool bShow)
{
	if (Crosshair)
	{
		Crosshair->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::SetCrosshairPosition(float x, float y)
{
	if (Crosshair)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Crosshair->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}

void UForgingWidget::UpdateHammerBar_0(float Progress)
{
	if (HammerBar_0)
	{
		HammerBar_0->SetPercent(Progress);
	}
}

void UForgingWidget::ShowHammerBar_0(bool bShow)
{
	if (HammerBar_0)
	{
		HammerBar_0->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::SetHammerBar_0Color(const FLinearColor& NewColor)
{
	if (HammerBar_0)
	{
		//FProgressBarStyle BarStyle = HammerBar_0->WidgetStyle;
		//BarStyle.FillImage.TintColor = FSlateColor(NewColor);
		//HammerBar_0->SetWidgetStyle(BarStyle);
	}
}

void UForgingWidget::SetHammerBar_0Target(float NewTarget)
{
	// Implementation depends on how the target is represented in the UI.
	// This is a placeholder for setting a target marker on the progress bar.
}

void UForgingWidget::SetHammerBar_0Position(float x, float y)
{
	if (HammerBar_0)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HammerBar_0->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}

void UForgingWidget::ShowTarget_0(bool bShow)
{
	if (Target_0)
	{
		Target_0->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::SetTarget_0Position(float x, float y)
{
	if (Target_0)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Target_0->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}

FVector2D UForgingWidget::GetCanvasSize() const
{
	// Return the resolution of the canvas panel
	if (Canvas)
	{
		return Canvas->GetCachedGeometry().GetLocalSize();
	}
	return FVector2D::ZeroVector;
}