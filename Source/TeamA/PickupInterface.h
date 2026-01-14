#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

// This class is required for reflection
UINTERFACE(Blueprintable)
class TEAMA_API UPickupInterface : public UInterface
{
    GENERATED_BODY()
};

// Actual interface
class TEAMA_API IPickupInterface
{
    GENERATED_BODY()

public:

    // Function callable from Blueprints and usable as a Message
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
    void CanBePickedUp(bool& bCanPickup,
        APickup*& PickupActor) const;
};
