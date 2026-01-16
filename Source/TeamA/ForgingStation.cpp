// Fill out your copyright notice in the Description page of Project Settings.

#include "ForgingStation.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "ForgingWidget.h"


AForgingStation::AForgingStation()
{
	PrimaryActorTick.bCanEverTick = true;

	isEntered = false;


}

void AForgingStation::BeginPlay()
{
	Super::BeginPlay();
}

// Enter and Exit

void AForgingStation::Enter_Implementation(ACharacter* Character)
{
	// Ensure we have a valid local player controller and bind input
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

	// Get current project by checking the first project in the workstation's inventory
	CurrentProject = nullptr;
	if (Inventory.Num() > 0)
	{
		CurrentProject = Cast<AProject>(Inventory[0]);
	}

	if (!CurrentProject)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForgingStation: No current project found in inventory"));
	}
	else 
	{
		CurrentForgingPattern = CurrentProject->ForgingPattern;
	}



	isEntered = true;
	isForging = false;


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

		if(!CurrentProject)
		{
			ForgingWidgetInstance->UpdateForgePrompt(TEXT("Insert a project to begin"));
		}
		else if(CurrentProject->bIsForged)
		{
			ForgingWidgetInstance->UpdateForgePrompt(TEXT("Project already forged"));
		}
		else {
			ForgingWidgetInstance->UpdateForgePrompt(TEXT("Press Space to start forging"));
		}


		ForgingWidgetInstance->ShowForgePrompt(true);

		ForgingWidgetInstance->ShowHammerBar_0(false);
		ForgingWidgetInstance->ShowHammerBar_1(false);
		ForgingWidgetInstance->ShowHammerBar_2(false);

		ForgingWidgetInstance->ShowTarget_0(false);
		ForgingWidgetInstance->ShowTarget_1(false);
		ForgingWidgetInstance->ShowTarget_2(false);

	}
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

		if (isForging)
		{
			float FillSpeed = 1.0f / HammerFillDuration;
			CurrentHammerFill += DeltaTime * FillSpeed;
			CurrentHammerFill = FMath::Clamp(CurrentHammerFill, 0.0f, 1.0f);

			// Update UI
			if (CurrentHammerIndex == 0)
			{
				ForgingWidgetInstance->UpdateHammerBar_0(CurrentHammerFill);
			}
			else if (CurrentHammerIndex == 1)
			{
				ForgingWidgetInstance->UpdateHammerBar_1(CurrentHammerFill);
			}
			else if (CurrentHammerIndex == 2)
			{
				ForgingWidgetInstance->UpdateHammerBar_2(CurrentHammerFill);
			}
		}


	}
}


// When Space is pressed
void AForgingStation::StartForgingSequence()
{

	


	if (isForging)
	{
		return;
	}
	if (!CurrentProject)
	{
		UE_LOG(LogTemp, Warning, TEXT("No project to forge"));
		return;
	}
	if (CurrentProject->bIsForged)
	{
		UE_LOG(LogTemp, Warning, TEXT("Project already forged"));
		return;
	}

	if (CurrentProject->forgingProgress >= 1.0f)
	{
		CurrentProject->bIsForged = true;
		ForgingWidgetInstance->UpdateForgePrompt(TEXT("Project fully forged"));
		ForgingWidgetInstance->ShowForgePrompt(true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("FORGING"));
	ForgingWidgetInstance->ShowForgePrompt(false);

	//Create an array of floats equal to the forging pattern length
	TArray<float> TargetPositions;
	int PatternLength = CurrentForgingPattern.Num();
	
	// Create target positions based on pattern length, between 0.1 and 0.9, spaced at least 0.1 apart
	TargetPositions.SetNum(PatternLength);
	for (int i = 0; i < PatternLength; i++)
	{
		float Position;
		bool bIsValidPosition;
		int Attempts = 0;
		do
		{
			bIsValidPosition = true;
			Position = FMath::FRandRange(0.1f, 0.9f);
			// Check against existing positions for minimum spacing
			for (int j = 0; j < i; j++)
			{
				if (FMath::Abs(Position - TargetPositions[j]) < 0.1f)
				{
					bIsValidPosition = false;
					break;
				}
			}
			Attempts++;
			if (Attempts > 100) // Prevent infinite loop
			{
				break;
			}
		} while (!bIsValidPosition);
		TargetPositions[i] = Position;
	}
	//Sort positions for left-to-right arrangement
	TargetPositions.Sort();

	//Get canvas size to set target position
	FVector2D ViewportSize = ForgingWidgetInstance->GetCanvasSize();

	float TargetPositionY = ViewportSize.Y * 0.4f; // Center
	float HammerBarPositionY = ViewportSize.Y * 0.8f; // Near bottom

	if (PatternLength > 0)
	{
		float TargetPositionX = ViewportSize.X * TargetPositions[0];
		ForgingWidgetInstance->SetTarget_0Position(TargetPositionX, TargetPositionY);
		float HammerBarPositionX = TargetPositionX;
		ForgingWidgetInstance->SetHammerBar_0Position(HammerBarPositionX, HammerBarPositionY);
		ForgingWidgetInstance->ShowHammerBar_0(true);
		ForgingWidgetInstance->ShowTarget_0(true);
	}
	if (PatternLength > 1)
	{
		float TargetPositionX = ViewportSize.X * TargetPositions[1];
		ForgingWidgetInstance->SetTarget_1Position(TargetPositionX, TargetPositionY);
		float HammerBarPositionX = TargetPositionX;
		ForgingWidgetInstance->SetHammerBar_1Position(HammerBarPositionX, HammerBarPositionY);
		ForgingWidgetInstance->ShowHammerBar_1(true);
		ForgingWidgetInstance->ShowTarget_1(true);
	}
	if (PatternLength > 2)
	{
		float TargetPositionX = ViewportSize.X * TargetPositions[2];
		ForgingWidgetInstance->SetTarget_2Position(TargetPositionX, TargetPositionY);
		float HammerBarPositionX = TargetPositionX;
		ForgingWidgetInstance->SetHammerBar_2Position(HammerBarPositionX, HammerBarPositionY);
		ForgingWidgetInstance->ShowHammerBar_2(true);
		ForgingWidgetInstance->ShowTarget_2(true);
	}

	isForging = true;
	CurrentHammerIndex = 0;
	CurrentHammerFill = 0.0f;
	TotalHammerHits = CurrentForgingPattern.Num();

	// For now, fixed target
	CurrentTargetValue = 0.5f;

	BeginNextHammer();

}

void AForgingStation::ProcessHammerInput()
{
	if (!isForging || !CurrentProject || !ForgingWidgetInstance)
		return;

	// Get cursor position
	float MouseX, MouseY;
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->GetMousePosition(MouseX, MouseY))
		return;

	FVector2D CursorPos(MouseX, MouseY);
	FVector2D TargetPos = GetTargetScreenPosition(CurrentHammerIndex);

	// Evaluate scores
	EForgeHitQuality TimingQuality =
		EvaluateTiming(CurrentHammerFill, 0.5f);

	EForgeHitQuality PositionQuality =
		EvaluatePosition(CursorPos, TargetPos);

	EForgeHitQuality FinalQuality =
		CombineHitQuality(TimingQuality, PositionQuality);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Hammer %d hit at %.2f"),
		CurrentHammerIndex,
		CurrentHammerFill
	);

	// Apply forging progress
	CurrentProject->forgingProgress += ForgingProgressPerHit;
	CurrentProject->forgingProgress =
		FMath::Clamp(CurrentProject->forgingProgress, 0.0f, 1.0f);


	// Hide current bar
	if (CurrentHammerIndex == 0)
	{
		ForgingWidgetInstance->ShowHammerBar_0(false);
	}
	else if (CurrentHammerIndex == 1)
	{
		ForgingWidgetInstance->ShowHammerBar_1(false);
	}
	else if (CurrentHammerIndex == 2)
	{
		ForgingWidgetInstance->ShowHammerBar_2(false);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Hit %d - Timing: %d | Position: %d | Final: %d | Progress: %.0f%%"),
		CurrentHammerIndex,
		(int32)TimingQuality,
		(int32)PositionQuality,
		(int32)FinalQuality,
		CurrentProject->forgingProgress * 100.0f
	);

	float HitScore = 0.0f;

	switch (FinalQuality)
	{
	case EForgeHitQuality::Perfect:
		HitScore = PerfectHitScore;
		break;

	case EForgeHitQuality::Good:
		HitScore = GoodHitScore;
		break;

	case EForgeHitQuality::Bad:
		HitScore = BadHitScore;
		break;
	}

	CurrentProject->TotalForgeHits++;
	CurrentProject->TotalForgeScore += HitScore;



	CurrentHammerIndex++;
	BeginNextHammer();
}



void AForgingStation::BeginNextHammer()
{
	if (CurrentHammerIndex >= TotalHammerHits)
	{
		FinishForging();
		return;
	}

	CurrentHammerFill = 0.0f;

	// Show correct bar
	if (CurrentHammerIndex == 0)
	{
		ForgingWidgetInstance->ShowHammerBar_0(true);
	}
	else if (CurrentHammerIndex == 1)
	{
		ForgingWidgetInstance->ShowHammerBar_1(true);
	}
	else if (CurrentHammerIndex == 2)
	{
		ForgingWidgetInstance->ShowHammerBar_2(true);
	}
}

void AForgingStation::FinishForging()
{
	isForging = false;

	if (CurrentProject->forgingProgress >= 1.0f)
	{
		CurrentProject->bIsForged = true;
		CurrentProject->FinalizeForgingScore();

		ForgingWidgetInstance->UpdateForgePrompt(
			FString::Printf(
				TEXT("Forging complete! Score: %.0f"),
				CurrentProject->FinalForgeScore
			)
		);
	}
	else
	{
		ForgingWidgetInstance->UpdateForgePrompt(TEXT("Press Space to continue forging"));
	}

	ForgingWidgetInstance->ShowForgePrompt(true);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Forging sequence complete. Progress: %.0f%%"),
		CurrentProject->forgingProgress * 100.0f
	);
}






// Input Binding

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

EForgeHitQuality AForgingStation::EvaluateTiming(float FillValue, float TargetValue) const
{
	float Error = FMath::Abs(FillValue - TargetValue);

	if (Error <= TimingPerfectThreshold)
		return EForgeHitQuality::Perfect;

	if (Error <= TimingGoodThreshold)
		return EForgeHitQuality::Good;

	return EForgeHitQuality::Bad;
}

EForgeHitQuality AForgingStation::EvaluatePosition(
	const FVector2D& CursorPos,
	const FVector2D& TargetPos) const
{
	float Distance = FVector2D::Distance(CursorPos, TargetPos);

	if (Distance <= PositionPerfectPixels)
		return EForgeHitQuality::Perfect;

	if (Distance <= PositionGoodPixels)
		return EForgeHitQuality::Good;

	return EForgeHitQuality::Bad;
}



FVector2D AForgingStation::GetTargetScreenPosition(int32 Index) const
{
	switch (Index)
	{
	case 0: return ForgingWidgetInstance->GetTarget_0Position();
	case 1: return ForgingWidgetInstance->GetTarget_1Position();
	case 2: return ForgingWidgetInstance->GetTarget_2Position();
	default: return FVector2D::ZeroVector;
	}
}

EForgeHitQuality AForgingStation::CombineHitQuality(
	EForgeHitQuality Timing,
	EForgeHitQuality Position)
{
	return static_cast<EForgeHitQuality>(
		FMath::Max(
			static_cast<uint8>(Timing),
			static_cast<uint8>(Position)
		)
		);
}
