#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "Project.generated.h"

/**
 *
 */
	UCLASS()
	class TEAMA_API AProject : public APickup
{
	GENERATED_BODY()

public:
	AProject();

	//Skeletal mesh component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMesh;

	//Variables for Forging minigame
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsForged;
	TArray<float> ForgingPattern;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float forgingProgress;
	float TotalForgeScore;
	int32 TotalForgeHits;
	float FinalForgeScore;
	void FinalizeForgingScore();



	UFUNCTION(BlueprintNativeEvent)
	void ForgeModel();

protected:


};
