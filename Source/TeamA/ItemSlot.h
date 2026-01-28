// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemTypesEnum.h"
#include "Pickup.h"
#include "Components/BoxComponent.h"
#include "ItemSlot.generated.h"

UCLASS()
class TEAMA_API AItemSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSlot();

protected:

public:	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* SocketPoint;

    /** What item type this socket accepts (optional: any) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemTypesEnum AcceptedItemType;

    /** Is socket currently occupied */
    UPROPERTY(BlueprintReadOnly)
    bool bIsOccupied;

    /** The item currently attached */
    UPROPERTY(BlueprintReadOnly)
    APickup* AttachedItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* InteractionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HighlightMesh;


    /** Try to attach an item to this socket */
    UFUNCTION(BlueprintCallable)
    virtual bool AttachItem(APickup* Item);

    /** Detach the currently attached item */
    UFUNCTION(BlueprintCallable)
    virtual void DetachItem();

	/** Delete the currently attached item */
    UFUNCTION(BlueprintCallable)
    virtual void DeleteItem();

    UFUNCTION(BlueprintCallable)
    virtual APickup* TakeItem();

    UFUNCTION(BlueprintCallable)
	void ShowHighlight(bool bShow);

};
