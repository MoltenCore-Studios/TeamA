// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root scene component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	// Create mesh component
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionMesh"));
	CollisionMesh->SetupAttachment(RootComponent);

	// simulate physics
	CollisionMesh->SetSimulatePhysics(true);
	// Generate overlap events
	CollisionMesh->SetGenerateOverlapEvents(true);
	// Enable collision
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::CanBePickedUp_Implementation(
	bool& bCanPickup,
	APickup*& PickupActor
) const
{
	bCanPickup = true;
	PickupActor = const_cast<APickup*>(this);
}