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

protected:
	virtual void BeginPlay() override;

	AProject* CurrentProject;
	
	TArray<float> CurrentForgingPattern;
	
	bool isEntered;
	bool isForging;

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


};
