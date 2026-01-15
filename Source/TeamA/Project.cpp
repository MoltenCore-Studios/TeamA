#include "ItemTypesEnum.h"
#include "Project.h"

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