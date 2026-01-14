// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemTypesEnum.h"
#include "PickupInterface.h"
#include "Pickup.generated.h"

UCLASS()
class TEAMA_API APickup : public AActor, public IPickupInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Root scene component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* RootSceneComponent;

	// Mesh component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CollisionMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemTypesEnum ItemType;

	virtual void CanBePickedUp_Implementation(bool& bCanPickup,
		APickup*& PickupActor) const override;


};
