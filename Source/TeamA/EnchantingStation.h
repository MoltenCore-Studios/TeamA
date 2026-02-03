// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Workstation.h"
#include "InputAction.h"
#include "EnchantingStation.generated.h"


struct FRunePoint
{
	FVector2D Position;
	float Time;
};

/**
 * 
 */
UCLASS()
class TEAMA_API AEnchantingStation : public AWorkstation
{
	GENERATED_BODY()
	
public:

	static constexpr float CanvasSize = 512.f;

	//Cached Player Controller
	UPROPERTY()
	class APlayerController* CachedPC = nullptr;

	// Draw input action
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DrawRuneAction;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void Enter_Implementation(class ACharacter* PC) override;
	//virtual void Exit_Implementation(class ACharacter* PC) override;

	void BindInput(class APlayerController* PC);
	void UnbindInput();

	// Input

	// cached input component
	UPROPERTY()
	class UEnhancedInputComponent* CachedEnhancedInput = nullptr;

	// Start and stop drawing
	void StartDrawing();
	void StopDrawing();
	
};
