// Fill out your copyright notice in the Description page of Project Settings.

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "ForgingWidget.h"
#include "ForgingStation.h"

AForgingStation::AForgingStation()
{
	PrimaryActorTick.bCanEverTick = true;

	isEntered = false;


}

void AForgingStation::BeginPlay()
{
	Super::BeginPlay();
}

void AForgingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isEntered)
	{
		return;
	}

	if (ForgingWidgetInstance)
	{
		//Get mouse position and set crosshair position
		ForgingWidgetInstance->ShowCrosshair(true);
		float MouseX, MouseY;
		APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC && PC->GetMousePosition(MouseX, MouseY))
		{
			ForgingWidgetInstance->SetCrosshairPosition(MouseX, MouseY);
		}
	}

	// Implementation for starting the forging sequence
	if (CurrentProject)
	{
		

		if (CurrentProject->bIsForged)
		{
			// Already forged
			return;
		}

	}
}

void AForgingStation::BindInput(APlayerController* PC)
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
	);
}

void AForgingStation::UnbindInput()
{
	if (!CachedEnhancedInput) return;

	CachedEnhancedInput->ClearActionBindings();
	CachedEnhancedInput = nullptr;

	DisableInput(nullptr);
}



void AForgingStation::Enter_Implementation(ACharacter* Character)
{
	if (ForgingWidgetClass)
	{
		ForgingWidgetInstance = CreateWidget<UForgingWidget>(
			GetWorld(),
			ForgingWidgetClass
		);

		if (ForgingWidgetInstance)
		{
			ForgingWidgetInstance->AddToViewport();
		}

		ForgingWidgetInstance->UpdateForgePrompt(TEXT("Press Space to start forging"));

		ForgingWidgetInstance->ShowForgePrompt(true);
		ForgingWidgetInstance->ShowHammerBar_0(false);
		ForgingWidgetInstance->ShowTarget_0(false);

	}


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

	if (Subsystem && ForgingMappingContext)
	{
		Subsystem->AddMappingContext(
			ForgingMappingContext,
			ForgingMappingPriority
		);
	}

	BindInput(PC);
	isEntered = true;
	isForging = false;
}


void AForgingStation::Exit_Implementation(ACharacter* Character)
{
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

	if (Subsystem && ForgingMappingContext)
	{
		Subsystem->RemoveMappingContext(ForgingMappingContext);
	}

	UnbindInput();
	isEntered = false;

	// Unload forging UI
	if (ForgingWidgetInstance)
	{
		ForgingWidgetInstance->RemoveFromParent();
		ForgingWidgetInstance = nullptr;
	}
}

void AForgingStation::StartForgingSequence()
{
	UE_LOG(LogTemp, Warning, TEXT("FORGING"));
	ForgingWidgetInstance->ShowForgePrompt(false);



	//Get canvas size to set target position
	FVector2D ViewportSize = ForgingWidgetInstance->GetCanvasSize();

	// Random target position within the canvas
	float TargetPositionX = FMath::FRandRange(0.1f, 0.9f) * ViewportSize.X;
	float TargetPositionY = ViewportSize.Y / 2.0f; // Center vertically

	ForgingWidgetInstance->SetTarget_0Position(TargetPositionX, TargetPositionY);

	float HammerBarPositionX = TargetPositionX;
	float HammerBarPositionY = ViewportSize.Y * 0.8f; // Near bottom
	ForgingWidgetInstance->SetHammerBar_0Position(HammerBarPositionX, HammerBarPositionY);

	ForgingWidgetInstance->ShowHammerBar_0(true);
	ForgingWidgetInstance->ShowTarget_0(true);
	isForging = true;

}

void AForgingStation::ProcessHammerInput()
{
	if (!isForging)
	{
		return;
	}


	// Implementation for processing hammer input
	UE_LOG(LogTemp, Warning, TEXT("HAMMERING"));
	ForgingWidgetInstance->ShowForgePrompt(true);

	// Hide hammer bar and target for now
	ForgingWidgetInstance->ShowHammerBar_0(false);
	ForgingWidgetInstance->ShowTarget_0(false);


}

