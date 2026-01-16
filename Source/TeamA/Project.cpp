#include "Project.h"
#include "ItemTypesEnum.h"

AProject::AProject()
{
	ItemType = EItemTypesEnum::Project;


	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	//CollisionMesh->SetVisibility(false);

	// Attach SkeletalMesh to CollisionMesh
	SkeletalMesh->SetupAttachment(InteractionVolume);

	//Set default values for forging minigame
	bIsForged = false;
	forgingProgress = 0.0f;
	ForgingPattern = { 3, 3, 9 }; // Example pattern
}

void AProject::FinalizeForgingScore()
{
	if (TotalForgeHits <= 0)
	{
		FinalForgeScore = 0.0f;
		return;
	}

	// Average hit score 0–1
	float AverageScore = TotalForgeScore / TotalForgeHits;

	// Convert to percentage
	FinalForgeScore = FMath::Clamp(AverageScore * 100.0f, 0.0f, 100.0f);
}

void AProject::ForgeModel_Implementation()
{

}
