// Fill out your copyright notice in the Description page of Project Settings.

#include "ForgingStation.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "DrawDebugHelpers.h"
#include "Project.h"
#include "NiagaraSystem.h"
#include "ForgingTargetActor.h"
#include "Components/WidgetComponent.h"
#include <iostream>
#include <algorithm>
#include "ForgingWidget.h"
using namespace std;


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

	PC->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

	PC->SetInputMode(InputMode);



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

	CurrentTargetValue = 0.0f;


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

	PC->bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);


	isEntered = false;

	// Unload forging UI
	if (ForgingWidgetInstance)
	{
		ForgingWidgetInstance->RemoveFromParent();
		ForgingWidgetInstance = nullptr;

		// Clear targets
		for (AForgingTargetActor* Target : ActiveTargets)
		{
			if (Target)
				Target->Destroy();
		}
	}
}


void AForgingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isEntered)
	{
		return;
	}

	if (CurrentTargetValue <= 0) {
		ForgingWidgetInstance->SetForgeTargetVisible(false);
	}
	else {
		ForgingWidgetInstance->SetForgeTargetVisible(true);
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
			CurrentHammerFill = FMath::Clamp(CurrentHammerFill, -HammerFillDelay, 1.0f);
			CurrentTargetValue = CurrentForgingPattern.IsValidIndex(CurrentHammerIndex)
				? CurrentForgingPattern[CurrentHammerIndex]
				: 0.5f;

			// Update UI
			ForgingWidgetInstance->UpdateHammerBar_0( max(CurrentHammerFill, 0.0f));
			ForgingWidgetInstance->SetForgeTargetPercent(CurrentTargetValue);
			
			AForgingTargetActor* CurrentTarget = ActiveTargets.IsValidIndex(CurrentHammerIndex)
				? ActiveTargets[CurrentHammerIndex]
				: nullptr;
			if (CurrentTarget) {
				//Color change logic here
				CurrentTarget->SetTargetWidgetColor(FLinearColor::Green);
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

	// HERE
	USkeletalMeshComponent* BladeMesh = CurrentProject->SkeletalMesh;
	if (!BladeMesh || !TargetActorClass)
		return;

	// Clear old targets
	/*
	if (ActiveTargets.Num() > 0)
	{
		for (AForgingTargetActor* Target : ActiveTargets)
		{
			if (Target)
				Target->Destroy();
		}
	}*/
		
	ActiveTargets.Empty();

	// Get local bounds of blade
	FBox LocalBounds =
		BladeMesh->CalcBounds(FTransform::Identity).GetBox();

	float MinZ = LocalBounds.Min.Z;
	float MaxZ = LocalBounds.Max.Z;

	// Safety clamp (avoid hilt & tip)
	MinZ = FMath::Lerp(MinZ, MaxZ, 0.1f);
	MaxZ = FMath::Lerp(MinZ, MaxZ, 0.9f);


	//Create an array of random numbers between MinZ and MaxZ based on the pattern length
	TArray<float> RandomZPositions;
	for (int32 i = 0; i < PatternLength; i++)
	{
		float RandomZ = FMath::FRandRange(MinZ, MaxZ);
		RandomZPositions.Add(RandomZ);
	}
	// Sort the array from largest to smallest
	RandomZPositions.Sort([](float A, float B) {return A > B;});
	//Ensure that the positions are not too close to each other
	const float MinSeparation = (MaxZ - MinZ) / (PatternLength * 2);
	for (int32 i = 1; i < RandomZPositions.Num(); i++)
	{
		if (FMath::Abs(RandomZPositions[i] - RandomZPositions[i - 1]) < MinSeparation)
		{
			RandomZPositions[i] = RandomZPositions[i - 1] - MinSeparation;
			// Clamp to MinZ
			RandomZPositions[i] = FMath::Max(RandomZPositions[i], MinZ);
		}
	}





	for (int32 i = 0; i < PatternLength; i++)
	{
		float Alpha = FMath::FRandRange(0.1f, 0.9f);
		float LocalZ = RandomZPositions[i];

		AForgingTargetActor* Target =
			GetWorld()->SpawnActor<AForgingTargetActor>(TargetActorClass);

		if (!Target)
			continue;

		// Attach first
		Target->AttachToComponent(
			BladeMesh,
			FAttachmentTransformRules::KeepRelativeTransform
		);

		// Set RELATIVE transform
		Target->SetActorRelativeLocation(
			FVector(
				50.0f,  // X: blade surface
				20.0f,  // Y: lateral offset
				LocalZ  // Z: along blade
			)
		);

		Target->SetActorRelativeRotation(FRotator::ZeroRotator);

		ActiveTargets.Add(Target);
	}


	ForgingWidgetInstance->ShowHammerBar_0(true);
	ForgingWidgetInstance->UpdateHammerBar_0(0.0f);

	isForging = true;
	CurrentHammerIndex = 0;
	CurrentHammerFill = -HammerFillDelay;
	TotalHammerHits = CurrentForgingPattern.Num();

	// For now, fixed target
	CurrentTargetValue = CurrentForgingPattern[CurrentHammerIndex];

	BeginNextHammer();

}

void AForgingStation::ProcessHammerInput()
{
	if (!isForging || !CurrentProject || !ForgingWidgetInstance)
		return;


	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	
	FVector2D CursorPos;
	bool bHasCursor =
		UWidgetLayoutLibrary::GetMousePositionScaledByDPI(
			PC,
			CursorPos.X,
			CursorPos.Y
		);

	if (!bHasCursor)
		return;


	FVector HitWorldPos;
	if (!GetMouseWorldPosition(HitWorldPos))
		return;

	AForgingTargetActor* CurrentTarget = ActiveTargets.IsValidIndex(CurrentHammerIndex)
		? ActiveTargets[CurrentHammerIndex]
		: nullptr;


	if (!CurrentTarget)
		return;




	// Evaluate scores
	EForgeHitQuality TimingQuality =
		EvaluateTiming(CurrentHammerFill, CurrentTargetValue);

	EForgeHitQuality PositionQuality =
		EvaluateScreenPosition( HitWorldPos, CurrentTarget );


	EForgeHitQuality FinalQuality =
		CombineHitQuality(TimingQuality, PositionQuality);


	//PlayHammerAnimation(HitWorldPos, FinalQuality);
	PlayHammerAnimation(CurrentHammerIndex, FinalQuality);


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
	CurrentProject->ForgeModel();

	float EffectScale = 0.01f;

	switch (FinalQuality)
	{
	case EForgeHitQuality::Perfect:
		EffectScale = EffectScale * 1.3f;
		break;

	case EForgeHitQuality::Good:
		EffectScale *= 1.0f;
		break;

	case EForgeHitQuality::Bad:
		EffectScale *= 0.7f;
		break;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), //Spawn in world
		HitEffect, // Effect to spawn
		HitWorldPos, // At target location
		FRotator::ZeroRotator, // No rotation
		FVector(EffectScale), // Scale based on hit quality
		true, // Auto destroy
		true // Auto activate
	);



	// Hide current target
	CurrentTarget->Destroy();

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

	CurrentHammerFill = -HammerFillDelay;

	ForgingWidgetInstance->ShowHammerBar_0(true);
}

void AForgingStation::FinishForging()
{
	isForging = false;
	CurrentTargetValue = 0.0f;

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
		//Hide hammer bar
		ForgingWidgetInstance->ShowHammerBar_0(false);
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
EForgeHitQuality AForgingStation::EvaluateScreenPosition(
	const FVector& HitWorldPos,
	AForgingTargetActor* Target
) const
{


	if (!Target || !Target->TargetWidget)
		return EForgeHitQuality::Bad;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
		return EForgeHitQuality::Bad;

	// 1. Get mouse position
	FVector2D MousePos;
	if (!PC->GetMousePosition(MousePos.X, MousePos.Y))
		return EForgeHitQuality::Bad;

	// 2. Project target widget center to screen
	FVector2D TargetScreenPos;
	bool bProjected = PC->ProjectWorldLocationToScreen(
		Target->TargetWidget->GetComponentLocation(),
		TargetScreenPos,
		false
	);

	UE_LOG(LogTemp, Warning,
		TEXT("Mouse: %s | Target: %s"),
		*MousePos.ToString(),
		*TargetScreenPos.ToString()
	);


	if (!bProjected)
		return EForgeHitQuality::Bad;

	// 3. Measure 2D screen distance (pixels)
	float ScreenDistance = FVector2D::Distance(MousePos, TargetScreenPos);

	// 4. Score based on pixel radius
	if (ScreenDistance <= Target->PerfectRadius)
		return EForgeHitQuality::Perfect;

	if (ScreenDistance <= Target->GoodRadius)
		return EForgeHitQuality::Good;

	return EForgeHitQuality::Bad;
}

EForgeHitQuality AForgingStation::CombineHitQuality(EForgeHitQuality Timing, EForgeHitQuality Position) { return static_cast<EForgeHitQuality>(FMath::Max(static_cast<uint8>(Timing), static_cast<uint8>(Position))); }



bool AForgingStation::GetMouseWorldPosition(FVector& OutWorldPos) const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
		return false;

	FVector WorldOrigin, WorldDir;
	if (!PC->DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
		return false;

	FHitResult Hit;
	FVector End = WorldOrigin + WorldDir * 10000.0f;

	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		WorldOrigin,
		End,
		ECC_Visibility
	))
	{
		//Log all information about the hit
		OutWorldPos = Hit.Location;
		return true;
	}

	return false;
}
