// Fill out your copyright notice in the Description page of Project Settings.

#include "Workstation.h"
#include "DrawDebugHelpers.h"
#include "ItemSlot.h"
#include "Pickup.h"


// Sets default values
AWorkstation::AWorkstation()
{
	PrimaryActorTick.bCanEverTick = true;


    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(Root);
    InteractionVolume->SetBoxExtent(FVector(75.f));
    InteractionVolume->SetCollisionProfileName(TEXT("Trigger"));

    CameraPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPoint"));
    CameraPoint->SetupAttachment(Root);

	// Add a static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	

	// Set mesh to default cube
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	//Create Arrow Component
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(CameraPoint);

}

// Called when the game starts or when spawned
void AWorkstation::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AWorkstation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Draw debug box for interaction volume
	FVector BoxLocation = InteractionVolume->GetComponentLocation();
	FRotator BoxRotation = InteractionVolume->GetComponentRotation();
	FVector BoxExtent = InteractionVolume->GetScaledBoxExtent();
	/*
	DrawDebugBox(
		GetWorld(),
		BoxLocation,
		BoxExtent,
		BoxRotation.Quaternion(),
		FColor::Green,
		false,
		-1.f,
		0,
		2.f
	);*/


}

void AWorkstation::Enter_Implementation(ACharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Entered Workstation"));
}

void AWorkstation::Exit_Implementation(ACharacter* Character)
{

}

void AWorkstation::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
    OutResult.Location = CameraPoint->GetComponentLocation();
    OutResult.Rotation = CameraPoint->GetComponentRotation();
    OutResult.FOV = 90.f;
}
