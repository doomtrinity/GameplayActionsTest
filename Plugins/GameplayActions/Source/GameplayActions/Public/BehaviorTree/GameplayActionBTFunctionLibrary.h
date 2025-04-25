#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayActionBTFunctionLibrary.generated.h"

class UBehaviorTreeComponent;
class IGameplayActionInterface;
class AActor;

UCLASS(meta = (RestrictedToClasses = "BTNode"))
class GAMEPLAYACTIONS_API UGameplayActionBTFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// not exposed to blueprint on purpose, meant to be called by C++ only
	static IGameplayActionInterface* GetGameplayActionInterface(UBehaviorTreeComponent& OwnerComp);
	static IGameplayActionInterface* GetGameplayActionInterface(AActor* ActionOwner);
};
