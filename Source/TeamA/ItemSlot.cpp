#include "ItemSlot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Workstation.h"
#include "Pickup.h"

AItemSlot::AItemSlot()
{
    PrimaryActorTick.bCanEverTick = false;

    SocketPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SocketPoint"));
    RootComponent = SocketPoint;

    bIsOccupied = false;
    AttachedItem = nullptr;

    AcceptedItemType = EItemTypesEnum::Project; // Default allows any type

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionVolume->SetGenerateOverlapEvents(true);
    DrawDebugBox(GetWorld(), SocketPoint->GetComponentLocation(), FVector(20.f), FColor::Green, false, 2.f);

	//Set each collision channel to block
	InteractionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// Set specific channels to block
	InteractionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);


	// Setup highlight mesh and set it to be hidden by default
	HighlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HighlightMesh"));
    if (HighlightMesh)
    {
		//Set position of highlight mesh to be slightly above the socket point
		HighlightMesh->SetWorldLocation(SocketPoint->GetComponentLocation() + FVector(0.f, 0.f, 10.f));
        HighlightMesh->SetupAttachment(RootComponent);
		HighlightMesh->SetVisibility(false);
		//Disable collision for highlight mesh
		HighlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Scale independently of parent scale
		HighlightMesh->SetAbsolute(false, false, true);
        
	}

}

bool AItemSlot::AttachItem(APickup* Item)
{
    if (!Item || bIsOccupied)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttachItem failed: Invalid item or slot occupied."));
        return false;
    }
    // Type check
    if (Item->ItemType != AcceptedItemType)
    {
		UE_LOG(LogTemp, Warning, TEXT("AttachItem failed: Item type mismatch."));
        return false;
    }

    // Attach item to socket
    Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Item->InteractionVolume->SetSimulatePhysics(false);
    Item->InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    Item->AttachToComponent(
        SocketPoint,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale
    );

    Item->SetActorRelativeLocation(FVector::ZeroVector);
    Item->SetActorRelativeRotation(FRotator::ZeroRotator);

    AttachedItem = Item;
    bIsOccupied = true;

	//Check if parent actor is a workstation
	AActor* ParentActor = GetAttachParentActor();
	if (ParentActor)
	{
		//Log parent actor name
		UE_LOG(LogTemp, Log, TEXT("ItemSlot parent actor: %s"), *ParentActor->GetName());
		//If it is, notify the workstation that an item has been attached
		AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);
        
		if (Workstation)
		{
			UE_LOG(LogTemp, Log, TEXT("Notifying workstation of attached item: %s"), *Item->GetName());
			Workstation->Inventory.Add(Item);
			//LOG current inventory count
			UE_LOG(LogTemp, Log, TEXT("Workstation inventory count: %d"), Workstation->Inventory.Num());
		}
	}


    return true;
}

void AItemSlot::DetachItem()
{
    if (!AttachedItem)
        return;

    APickup* Item = AttachedItem;

    AActor* ParentActor = GetAttachParentActor();
    if (ParentActor)
    {
        //If it is, notify the workstation that an item has been removed
        AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);
        if (Workstation)
        {
            Workstation->Inventory.Remove(Item);
        }
    }

    AttachedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    AttachedItem->InteractionVolume->SetSimulatePhysics(true);
    AttachedItem->InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    AttachedItem = nullptr;
    bIsOccupied = false;

}

void AItemSlot::DeleteItem()
{
    if (!AttachedItem)
        return;

    APickup* Item = AttachedItem;

    AActor* ParentActor = GetAttachParentActor();
    if (ParentActor)
    {
        //If it is, notify the workstation that an item has been removed
        AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);
        if (Workstation)
        {
            Workstation->Inventory.Remove(Item);
        }
    }

    AttachedItem->Destroy();
    AttachedItem = nullptr;
    bIsOccupied = false;

 
}

APickup* AItemSlot::TakeItem()
{
    if (!AttachedItem)
        return nullptr;

    APickup* Item = AttachedItem;

    AttachedItem = nullptr;
    bIsOccupied = false;

	//Check if parent actor is a workstation
	AActor* ParentActor = GetAttachParentActor();
	if (ParentActor)
	{
		//If it is, notify the workstation that an item has been removed
		AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);
		if (Workstation)
		{
			Workstation->Inventory.Remove(Item);
		}
	}

    return Item;
}

void AItemSlot::ShowHighlight(bool bShow)
{
    if (HighlightMesh)
    {
        HighlightMesh->SetVisibility(bShow);
    }
}