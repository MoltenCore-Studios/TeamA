// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Workstation.h"
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
	TArray<FRunePoint> Stroke;

protected:
	//virtual void Tick(float DeltaTime) override;
	virtual void Enter_Implementation(class ACharacter* PC) override;
	//virtual void Exit_Implementation(class ACharacter* PC) override;

	void BindInput(class APlayerController* PC);
	void UnbindInput();

	// Input
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* StartForgingAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* HammerAction;

	// cached input component
	UPROPERTY()
	class UEnhancedInputComponent* CachedEnhancedInput = nullptr;
};
