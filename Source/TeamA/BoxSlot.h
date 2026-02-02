// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSlot.h"
#include "BoxSlot.generated.h"

/**
 * 
 */
UCLASS()
class TEAMA_API ABoxSlot : public AItemSlot
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABoxSlot();

protected:

public:

	UPROPERTY(BlueprintReadOnly)
	TArray<APickup*> ContainedItems;

	UPROPERTY(BlueprintReadWrite)
	int ItemMax;

	virtual bool AttachItem(APickup* Item) override;

	virtual void DetachItem() override;

	virtual void DeleteItem() override;


	APickup* TakeItem() override;

	
};
