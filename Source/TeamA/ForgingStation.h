// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Workstation.h"
#include "Project.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ForgingWidget.h"
#include "ForgingStation.generated.h"

/**
 * 
 */

class UInputComonponent;

UENUM(BlueprintType)
enum class EForgeHitQuality : uint8
{
	Perfect,
	Good,
	Bad
};


UCLASS()
class TEAMA_API AForgingStation : public AWorkstation
{
	GENERATED_BODY()

public:
	AForgingStation();

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* StartForgingAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* HammerAction;

	void StartForgingSequence();
	void ProcessHammerInput();

	// How much forging progress per successful hit
	UPROPERTY(EditAnywhere, Category = "Forging")
	float ForgingProgressPerHit = 0.05f; // 5%

	// Scoring tolerances
	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float TimingPerfectThreshold = 0.05f; // ±5%

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float TimingGoodThreshold = 0.15f; // ±15%

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float PositionPerfectPixels = 20.f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float PositionGoodPixels = 60.f;

	// Forging score tracking
	UPROPERTY(BlueprintReadOnly)
	int32 TotalForgeHits = 0;

	UPROPERTY(BlueprintReadOnly)
	float TotalForgeScore = 0.0f;

	// Final normalized score (0–100)
	UPROPERTY(BlueprintReadOnly)
	float FinalForgeScore = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float PerfectHitScore = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float GoodHitScore = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Forging|Scoring")
	float BadHitScore = 0.2f;

	

protected:
	virtual void BeginPlay() override;

	AProject* CurrentProject;
	
	TArray<float> CurrentForgingPattern;
	
	bool isEntered;
	bool isForging;
	
	// Forging progression
	int32 CurrentHammerIndex = 0;

	// Current bar fill (0–1)
	float CurrentHammerFill = 0.0f;

	// How fast bars fill (seconds to full)
	UPROPERTY(EditAnywhere, Category = "Forging")
	float HammerFillDuration = 1.5f;

	// Cached pattern length
	int32 TotalHammerHits = 0;

	// Target value (0–1) — later derived from pattern
	float CurrentTargetValue = 0.5f;








	// Cached target positions (generated at start)
	TArray<float> CachedTargetPositions;

	void BeginNextHammer();
	void FinishForging();
	EForgeHitQuality EvaluateTiming(float FillValue, float TargetValue) const;
	FVector2D GetTargetScreenPosition(int32 Index) const;
	EForgeHitQuality EvaluatePosition(const FVector2D& CursorPos,const FVector2D& TargetPos) const;
	EForgeHitQuality CombineHitQuality(EForgeHitQuality Timing, EForgeHitQuality Position);


	// Tuning
	UPROPERTY(EditAnywhere, Category = "Forging")
	float BaseChargeSpeed = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Forging")
	float PerfectWindow = 0.08f;

	UPROPERTY(EditAnywhere, Category = "Forging")
	float AcceptableWindow = 0.15f;


	UPROPERTY()
	UEnhancedInputComponent* CachedEnhancedInput = nullptr;

	void BindInput(APlayerController* PC);
	void UnbindInput();

	// Widget instance (runtime)
	UPROPERTY()
	UForgingWidget* ForgingWidgetInstance;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Enter_Implementation(class ACharacter* Character) override;
	virtual void Exit_Implementation(class ACharacter* Character) override;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* ForgingMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 ForgingMappingPriority = 2;

	// UI Widget for forging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> ForgingWidgetClass;

	bool GetBladeScreenBounds(float& OutMinX, float& OutMaxX) const;

};
