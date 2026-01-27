// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;


	InteractionVolume->SetCollisionProfileName(TEXT("BlockAll"));
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InteractionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	InteractionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InteractionVolume->SetGenerateOverlapEvents(true);
	InteractionVolume->SetSimulatePhysics(true);
	InteractionVolume->SetEnableGravity(true);

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

void APickup::OnPickedUp_Implementation()
{
	// You can add custom logic here for when the item is picked up
	// For example, play a sound or spawn a particle effect
}
