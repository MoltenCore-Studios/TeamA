// Copyright Epic Games, Inc. All Rights Reserved.

#include "TeamACharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ItemSlot.h"
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

	HoldPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HoldPoint"));
	HoldPoint->SetupAttachment(FirstPersonCameraComponent);
	HoldPoint->SetRelativeLocation(FVector(100.f, 0.f, -10.f));

	// can tick every frame
	PrimaryActorTick.bCanEverTick = true;
}

// Begin play
void ATeamACharacter::BeginPlay()
{
	Super::BeginPlay();
	if (FirstPersonWidgetClass)
	{
		FirstPersonWidgetInstance = CreateWidget<UFirstPersonWidget>(
			GetWorld(),
			FirstPersonWidgetClass
		);

		if (FirstPersonWidgetInstance)
		{
			FirstPersonWidgetInstance->AddToViewport();
		}

		FirstPersonWidgetInstance->ShowCrosshair(true);
		FirstPersonWidgetInstance->ShowInteractPrompt(false);
		FirstPersonWidgetInstance->ShowEnterPrompt(false);
	}
}

// tick
void ATeamACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateInteractPrompt();

	UpdateItemSlotHighlight();
	
}

void ATeamACharacter::UpdateItemSlotHighlight()
{

	TArray<AActor*> FoundSlots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemSlot::StaticClass(), FoundSlots);
	if (HeldItem)
	{
		// Iterate through slots and highlight valid ones
		for (AActor* Actor : FoundSlots)
		{
			AItemSlot* Slot = Cast<AItemSlot>(Actor);
			if (Slot)
			{
				if (Slot->AcceptedItemType == HeldItem->ItemType && !Slot->bIsOccupied)
				{
					// Highlight valid slot
					if (Slot->HighlightMesh)
					{
						Slot->ShowHighlight(true);
					}
				}
				else
				{
					// Remove highlight from invalid slot
					if (Slot->HighlightMesh)
					{
						Slot->ShowHighlight(false);
					}
				}
			}
		}
	}
	else
	{
		// No held item, remove all highlights
		for (AActor* Actor : FoundSlots)
		{
			AItemSlot* Slot = Cast<AItemSlot>(Actor);
			if (Slot && Slot->HighlightMesh)
			{
				Slot->ShowHighlight(false);
			}
		}
	}
}

void ATeamACharacter::UpdateInteractPrompt()
{
	if (!FirstPersonWidgetInstance)
	{
		return;
	}

	//if in a workstation, no prompts
	if (CurrentWorkstation)
	{
		FirstPersonWidgetInstance->ShowInteractPrompt(false);
		FirstPersonWidgetInstance->ShowEnterPrompt(false);
		return;
	}

	if (OverlappingWorkstation) {
		//Show interact prompt
		FirstPersonWidgetInstance->ShowEnterPrompt(true);
		FirstPersonWidgetInstance->UpdateEnterPrompt(TEXT("Press 'E' to enter"));
	}
	else {
		FirstPersonWidgetInstance->ShowEnterPrompt(false);
	}


	if (HeldItem)
	{
		// Line trace or overlap to detect socket
		FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * PickupRange);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(HeldItem);

		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel5, Params);

		if (bHit)
		{
			AItemSlot* Slot = Cast<AItemSlot>(Hit.GetActor());
			if (Slot && Slot->AcceptedItemType == HeldItem->ItemType)
			{
				FirstPersonWidgetInstance->ShowInteractPrompt(true);
				FirstPersonWidgetInstance->UpdateInteractPrompt(TEXT("Press 'Left Click' to place item"));
				return;
			}
		}
	}
	else {
		
		APickup* PickupInView = GetPickupInViewNoTake();
		if (PickupInView)
		{
			FirstPersonWidgetInstance->ShowInteractPrompt(true);
			FirstPersonWidgetInstance->UpdateInteractPrompt(TEXT("Press 'Left Click' to pick up item"));
			return;
		}
	}


	FirstPersonWidgetInstance->ShowInteractPrompt(false);
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

		// Pickup
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &ATeamACharacter::ItemInteract);

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
		//UE_LOG(LogTeamA, Log, TEXT("Overlapping Workstation: %s"), *OverlappingWorkstation->GetName());
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
	//Disable look rotation
	GetController()->SetIgnoreLookInput(true);


	// Switch camera
	FViewTargetTransitionParams Params;
	Params.BlendTime = 0.35f;

	APlayerController* PC = Cast<APlayerController>(GetController());
	PC->SetViewTarget(CurrentWorkstation, Params);

	CurrentWorkstation->Enter(this);

	// Disable item pickup input while at workstation

	//Hide UI crosshair and interact prompt
	if (FirstPersonWidgetInstance)
	{
		FirstPersonWidgetInstance->ShowCrosshair(false);
		FirstPersonWidgetInstance->ShowInteractPrompt(false);
	}


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
	//Enable look rotation
	GetController()->SetIgnoreLookInput(false);

	CurrentWorkstation->Exit(this);
	CurrentWorkstation = nullptr;

	//Show the UI crosshair
	if (FirstPersonWidgetInstance)
	{
		FirstPersonWidgetInstance->ShowCrosshair(true);
	}
}




void ATeamACharacter::ItemInteract()
{
	if (CurrentWorkstation) { return; }

	if (HeldItem)
	{
		DropItem();
		return;
	}
	else 
	{
		PickupItem();
		return;
	}
}

void ATeamACharacter::DropItem()
{
	// Line trace or overlap to detect socket
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * PickupRange);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(HeldItem);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel5, Params);
	// Log hit result
	UE_LOG(LogTeamA, Log, TEXT("Line Trace Hit: %s"), bHit ? TEXT("True") : TEXT("False"));


	if (bHit)
	{
		UE_LOG(LogTeamA, Log, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());
		AItemSlot* Slot = Cast<AItemSlot>(Hit.GetActor());
		UE_LOG(LogTeamA, Log, TEXT("Casting to AItemSlot: %s"), Slot ? TEXT("Success") : TEXT("Failed"));
		if (Slot && Slot->AttachItem(HeldItem))
		{
			// Successfully attached to socket
			UE_LOG(LogTeamA, Log, TEXT("Item attached to socket: %s"), *Slot->GetName());
			HeldItem = nullptr;
			return;
		}
	}

	// If no socket hit, drop normally
	HeldItem->GetRootComponent()->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);


	HeldItem->InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeldItem->InteractionVolume->SetSimulatePhysics(true);
	HeldItem->InteractionVolume->SetEnableGravity(true);

	HeldItem = nullptr;
}

void ATeamACharacter::PickupItem()
{
	APickup* Pickup = GetPickupInView();
	if (!Pickup) { return; }

	HeldItem = Pickup;

	// Disable physics
	Pickup->InteractionVolume->SetSimulatePhysics(false);
	Pickup->InteractionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Pickup->InteractionVolume->SetEnableGravity(false);

	// Attach to hold point
	Pickup->AttachToComponent(
		HoldPoint,
		FAttachmentTransformRules::FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		));

	Pickup->SetActorRelativeLocation(FVector::ZeroVector);
	Pickup->SetActorRelativeRotation(FRotator::ZeroRotator);
	Pickup->OnPickedUp();
}

APickup* ATeamACharacter::GetPickupInView()
{

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * 300.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = false;
	// Check for item slot
	bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_GameTraceChannel5,
		Params
	);

	if (bHit) {
		if (AItemSlot* Slot = Cast<AItemSlot>(Hit.GetActor()))
		{
			if (Slot->bIsOccupied)
			{
				APickup* Item = Slot->TakeItem();
				if (Item)
				{
					return Item;
				}
			}
		}
	}
	bHit = false;

	bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_GameTraceChannel4,
		Params
	);

	if (bHit)
	{
		return Cast<APickup>(Hit.GetActor());
	}

	return nullptr;
}

APickup* ATeamACharacter::GetPickupInViewNoTake() 
{
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * 300.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = false;
	// Check for item slot
	bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_GameTraceChannel5,
		Params
	);

	if (bHit) {
		if (AItemSlot* Slot = Cast<AItemSlot>(Hit.GetActor()))
		{
			if (Slot->bIsOccupied)
			{
				APickup* Item = Slot->AttachedItem;
				if (Item)
				{
					return Item;
				}
			}
		}
	}
	bHit = false;

	bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_GameTraceChannel4,
		Params
	);

	if (bHit)
	{
		return Cast<APickup>(Hit.GetActor());
	}

	return nullptr;
}