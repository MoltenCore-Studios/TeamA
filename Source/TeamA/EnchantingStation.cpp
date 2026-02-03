// Fill out your copyright notice in the Description page of Project Settings.


#include "EnchantingStation.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"


void AEnchantingStation::Enter_Implementation(ACharacter* Character)
{
	Stroke.Empty();

	APlayerController* PC = Character
		? Cast<APlayerController>(Character->GetController())
		: nullptr;

	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);

	BindInput(PC);

	PC->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

	PC->SetInputMode(InputMode);
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
	/*
	CachedEnhancedInput->BindAction(
		StartForgingAction,
		ETriggerEvent::Started,
		this,
		&AForgingStation::StartForgingSequence
	);

	CachedEnhancedInput->BindAction(
		HammerAction,
		ETriggerEvent::Started,
		this,
		&AForgingStation::ProcessHammerInput
	);*/
}

void AEnchantingStation::UnbindInput()
{
	if (!CachedEnhancedInput) return;

	CachedEnhancedInput->ClearActionBindings();
	CachedEnhancedInput = nullptr;

	DisableInput(nullptr);
}