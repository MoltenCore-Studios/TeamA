// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxSlot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Workstation.h"
#include "Pickup.h"

ABoxSlot::ABoxSlot()
{
	PrimaryActorTick.bCanEverTick = false;
	ContainedItems = TArray<APickup*>();
}

bool ABoxSlot::AttachItem(APickup* Item)
{
	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachItem failed: Invalid item."));
		return false;
	}
	if (Item->ItemType != AcceptedItemType)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachItem failed: Item type mismatch."));
		return false;
	}

	if (ContainedItems.Num() >= ItemMax)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachItem failed: BoxSlot is full."));
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
	ContainedItems.Add(Item);
	//Hide the current attached item
	if (AttachedItem)
	{
		AttachedItem->SetActorHiddenInGame(true);
	}
	

	AttachedItem = Item;
	bIsOccupied = true;

	//Check if parent actor is a workstation
	AActor* ParentActor = GetAttachParentActor();
	if (ParentActor)
	{
		//If it is, notify the workstation that an item has been attached
		AWorkstation* Workstation = Cast<AWorkstation>(ParentActor);

		if (Workstation)
		{
			Workstation->Inventory.Add(Item);
		}
	}

	return true;
}

void ABoxSlot::DetachItem()
{
	if (ContainedItems.Num() == 0)
		return;
	APickup* Item = ContainedItems.Pop();
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
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->InteractionVolume->SetSimulatePhysics(true);
	Item->InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (ContainedItems.Num() > 0)
	{
		AttachedItem = ContainedItems.Last();
		AttachedItem->SetActorHiddenInGame(false);
	}
	else
	{
		AttachedItem = nullptr;
		bIsOccupied = false;
	}
	return;
}

void ABoxSlot::DeleteItem()
{
	if (ContainedItems.Num() == 0)
		return;
	APickup* Item = ContainedItems.Pop();
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
	Item->Destroy();
	if (ContainedItems.Num() > 0)
	{
		AttachedItem = ContainedItems.Last();
		AttachedItem->SetActorHiddenInGame(false);
	}
	else
	{
		AttachedItem = nullptr;
		bIsOccupied = false;
	}
}

APickup* ABoxSlot::TakeItem()
{
	if (ContainedItems.Num() == 0)
		return nullptr;
	APickup* Item = ContainedItems.Pop();
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
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->InteractionVolume->SetSimulatePhysics(true);
	Item->InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (ContainedItems.Num() > 0)
	{
		AttachedItem = ContainedItems.Last();
		AttachedItem->SetActorHiddenInGame(false);
	}
	else
	{
		AttachedItem = nullptr;
		bIsOccupied = false;
	}
	return Item;
}