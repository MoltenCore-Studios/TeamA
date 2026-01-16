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

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HammerBar_1;

	void UpdateHammerBar_1(float Progress);
	void ShowHammerBar_1(bool bShow);
	void SetHammerBar_1Color(const FLinearColor& NewColor);
	void SetHammerBar_1Target(float NewTarget);
	void SetHammerBar_1Position(float x, float y);

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HammerBar_2;

	void UpdateHammerBar_2(float Progress);
	void ShowHammerBar_2(bool bShow);
	void SetHammerBar_2Color(const FLinearColor& NewColor);
	void SetHammerBar_2Target(float NewTarget);
	void SetHammerBar_2Position(float x, float y);


	// ===== Targets =====

	UPROPERTY(meta = (BindWidget))
	class UImage* Target_0;

	void ShowTarget_0(bool bShow);
	void SetTarget_0Position(float x, float y);
	FVector2D GetTarget_0Position() const;


	UPROPERTY(meta = (BindWidget))
	class UImage* Target_1;

	void ShowTarget_1(bool bShow);
	void SetTarget_1Position(float x, float y);
	FVector2D GetTarget_1Position() const;

	UPROPERTY(meta = (BindWidget))
	class UImage* Target_2;

	void ShowTarget_2(bool bShow);
	void SetTarget_2Position(float x, float y);
	FVector2D GetTarget_2Position() const;
	



};
