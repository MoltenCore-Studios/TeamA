// Fill out your copyright notice in the Description page of Project Settings.

#include "ForgingTargetActor.h"
#include "Components/WidgetComponent.h"

// Sets default values
AForgingTargetActor::AForgingTargetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	TargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetWidget"));
	SetRootComponent(TargetWidget);

	TargetWidget->SetWidgetSpace(EWidgetSpace::World);
	TargetWidget->SetDrawSize(FVector2D(32, 32));
	TargetWidget->SetPivot(FVector2D(0.5f, 0.5f));


}

// Called when the game starts or when spawned
void AForgingTargetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AForgingTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AForgingTargetActor::SetTargetWidgetColor(const FLinearColor& NewColor)
{
	if (TargetWidget)
	{
		TargetWidget->SetTintColorAndOpacity(NewColor);
	}
}
