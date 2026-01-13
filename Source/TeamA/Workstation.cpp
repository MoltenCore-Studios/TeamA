// Fill out your copyright notice in the Description page of Project Settings.


#include "Workstation.h"

// Sets default values
AWorkstation::AWorkstation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

}

void AWorkstation::Enter(ACharacter* Character)
{
}

void AWorkstation::Exit(ACharacter* Character)
{
}

void AWorkstation::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
    OutResult.Location = CameraPoint->GetComponentLocation();
    OutResult.Rotation = CameraPoint->GetComponentRotation();
    OutResult.FOV = 90.f;
}
