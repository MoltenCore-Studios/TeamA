// Copyright Epic Games, Inc. All Rights Reserved.

#include "TeamACharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TeamA.h"

ATeamACharacter::ATeamACharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// Set up overlap events
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATeamACharacter::OnOverlapBegin);

	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ATeamACharacter::OnOverlapEnd);

}

void ATeamACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATeamACharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATeamACharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATeamACharacter::LookInput);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATeamACharacter::Interact);
		EnhancedInputComponent->BindAction(ExitWorkstationAction, ETriggerEvent::Triggered, this, &ATeamACharacter::ExitWorkstation);

	}
	else
	{
		UE_LOG(LogTeamA, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ATeamACharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ATeamACharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ATeamACharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATeamACharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}


void ATeamACharacter::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	OverlappingWorkstation = Cast<AWorkstation>(OtherActor);
	if (OverlappingWorkstation)
	{
		// You can add additional logic here if needed when overlapping begins
		UE_LOG(LogTeamA, Log, TEXT("Overlapping Workstation: %s"), *OverlappingWorkstation->GetName());
	}
}

void ATeamACharacter::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == OverlappingWorkstation)
	{
		OverlappingWorkstation = nullptr;
	}
}


void ATeamACharacter::Interact()
{
	if (CurrentWorkstation || !OverlappingWorkstation)
		return;

	CurrentWorkstation = OverlappingWorkstation;

	// Disable movement
	GetCharacterMovement()->DisableMovement();

	// Switch camera
	FViewTargetTransitionParams Params;
	Params.BlendTime = 0.35f;

	APlayerController* PC = Cast<APlayerController>(GetController());
	PC->SetViewTarget(CurrentWorkstation, Params);

	CurrentWorkstation->Enter(this);
}

void ATeamACharacter::ExitWorkstation()
{
	if (!CurrentWorkstation)
		return;

	APlayerController* PC = Cast<APlayerController>(GetController());

	FViewTargetTransitionParams Params;
	Params.BlendTime = 0.35f;
	PC->SetViewTarget(this, Params);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	CurrentWorkstation->Exit(this);
	CurrentWorkstation = nullptr;
}
