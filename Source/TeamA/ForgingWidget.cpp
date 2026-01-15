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


// ===== Hammer Bars =====

void UForgingWidget::UpdateHammerBar_0(float Progress)
{
	if (HammerBar_0)
	{
		HammerBar_0->SetPercent(Progress);
	}
}

void UForgingWidget::UpdateHammerBar_1(float Progress)
{
	if (HammerBar_1)
	{
		HammerBar_1->SetPercent(Progress);
	}
}

void UForgingWidget::UpdateHammerBar_2(float Progress)
{
	if (HammerBar_2)
	{
		HammerBar_2->SetPercent(Progress);
	}
}

void UForgingWidget::ShowHammerBar_0(bool bShow)
{
	if (HammerBar_0)
	{
		HammerBar_0->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::ShowHammerBar_1(bool bShow)
{
	if (HammerBar_1)
	{
		HammerBar_1->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::ShowHammerBar_2(bool bShow)
{
	if (HammerBar_2)
	{
		HammerBar_2->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::SetHammerBar_0Color(const FLinearColor& NewColor)
{
	if (HammerBar_0)
	{

	}
}

void UForgingWidget::SetHammerBar_1Color(const FLinearColor& NewColor)
{
	if (HammerBar_1)
	{

	}
}

void UForgingWidget::SetHammerBar_2Color(const FLinearColor& NewColor)
{
	if (HammerBar_2)
	{

	}
}

void UForgingWidget::SetHammerBar_0Target(float NewTarget)
{
	// Implementation depends on how the target is represented in the UI.
	// This is a placeholder for setting a target marker on the progress bar.
}

void UForgingWidget::SetHammerBar_1Target(float NewTarget)
{
	// Implementation depends on how the target is represented in the UI.
	// This is a placeholder for setting a target marker on the progress bar.
}

void UForgingWidget::SetHammerBar_2Target(float NewTarget)
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

void UForgingWidget::SetHammerBar_1Position(float x, float y)
{
	if (HammerBar_1)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HammerBar_1->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}

void UForgingWidget::SetHammerBar_2Position(float x, float y)
{
	if (HammerBar_2)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HammerBar_2->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}


// ===== Targets =====

void UForgingWidget::ShowTarget_0(bool bShow)
{
	if (Target_0)
	{
		Target_0->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::ShowTarget_1(bool bShow)
{
	if (Target_1)
	{
		Target_1->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UForgingWidget::ShowTarget_2(bool bShow)
{
	if (Target_2)
	{
		Target_2->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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

void UForgingWidget::SetTarget_1Position(float x, float y)
{
	if (Target_1)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Target_1->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(x, y));
		}
	}
}

void UForgingWidget::SetTarget_2Position(float x, float y)
{
	if (Target_2)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Target_2->Slot);
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