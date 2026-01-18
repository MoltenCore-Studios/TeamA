// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForgingTargetActor.generated.h"

UCLASS()
class TEAMA_API AForgingTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AForgingTargetActor();



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWidgetComponent* TargetWidget;

	// Used for scoring
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PerfectRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GoodRadius = 35.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
