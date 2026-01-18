// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ForgingWidget.generated.h"

/**
 * 
 */
UCLASS()
class UForgingWidget : public UUserWidget
{
    GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Canvas;
	FVector2D GetCanvasSize() const;


    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ForgePrompt;

	void UpdateForgePrompt(const FString& NewText);
	void ShowForgePrompt(bool bShow);

	// ===== Hammer Bars =====
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HammerBar_0;

	void UpdateHammerBar_0(float Progress);
	void ShowHammerBar_0(bool bShow);
	void SetHammerBar_0Color(const FLinearColor& NewColor);
	void SetHammerBar_0Target(float NewTarget);
	void SetHammerBar_0Position(float x, float y);

};
