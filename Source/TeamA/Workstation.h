// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Workstation.generated.h"

UCLASS()
class TEAMA_API AWorkstation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorkstation();

	// Called when player enters 
	UFUNCTION(BlueprintNativeEvent, Category = "Workstation")
	void Enter(class ACharacter* Character);

	// Called when player exits 
	UFUNCTION(BlueprintNativeEvent, Category = "Workstation")
	void Exit(class ACharacter* Character);

	virtual void CalcCamera(
		float DeltaTime,
		struct FMinimalViewInfo& OutResult) override;

	// Array of pickup actors that are in this workstations inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<class APickup*> Inventory;

	//Arrow Component
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArrowComp;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* InteractionVolume;

	// Visible mesh for the workstation 
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// Camera location/rotation when player is using this station
	UPROPERTY(VisibleAnywhere)
	USceneComponent* CameraPoint;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
