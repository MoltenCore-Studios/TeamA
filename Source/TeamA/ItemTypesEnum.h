#pragma once

#include "CoreMinimal.h"
#include "ItemTypesEnum.generated.h"

/**
 * Custom enum to demonstrate how to expose enums to Blueprints.
 */
UENUM(BlueprintType)
enum class EItemTypesEnum : uint8
{
    Ore UMETA(DisplayName = "Ore"),
    Project UMETA(DisplayName = "Project"),
    Money UMETA(DisplayName = "Money")
};