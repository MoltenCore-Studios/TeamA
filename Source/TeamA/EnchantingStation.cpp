// Fill out your copyright notice in the Description page of Project Settings.


#include "EnchantingStation.h"
#include "InputActionValue.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"


void AEnchantingStation::Enter_Implementation(ACharacter* Character)
{
	APlayerController* PC = Character
		? Cast<APlayerController>(Character->GetController())
		: nullptr;

	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	BindInput(PC);

	PC->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

	PC->SetInputMode(InputMode);
	//EnableTick
	PrimaryActorTick.bCanEverTick = true;

	CachedPC = PC;
}


void AEnchantingStation::Tick(float DeltaTime)
{

}

void AEnchantingStation::StartDrawing()
{
	// Start drawing logic here
	UE_LOG(LogTemp, Log, TEXT("Started Drawing Rune"));
}

void AEnchantingStation::StopDrawing()
{
	// Stop drawing logic here
	UE_LOG(LogTemp, Log, TEXT("Stopped Drawing Rune"));
}





void AEnchantingStation::BindInput(APlayerController* PC)
{
	if (!PC) return;

	// Enable input gives us access to the PlayerController's InputComponent
	EnableInput(PC);



	CachedEnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!CachedEnhancedInput)
	{
		UE_LOG(LogTemp, Error, TEXT("ForgingStation: No EnhancedInputComponent"));
		return;
	}
	
	CachedEnhancedInput->BindAction(
		DrawRuneAction,
		ETriggerEvent::Started,
		this,
		&AEnchantingStation::StartDrawing
	);

	CachedEnhancedInput->BindAction(
		DrawRuneAction,
		ETriggerEvent::Completed,
		this,
		&AEnchantingStation::StopDrawing
	);
}

void AEnchantingStation::UnbindInput()
{
	if (!CachedEnhancedInput) return;

	CachedEnhancedInput->ClearActionBindings();
	CachedEnhancedInput = nullptr;

	DisableInput(nullptr);
}