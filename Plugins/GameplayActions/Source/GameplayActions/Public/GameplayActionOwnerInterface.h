#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GameplayActionOwnerInterface.generated.h"

class UActorComponent;


// this class does not need to be modified
UINTERFACE(MinimalAPI)
class UGameplayActionOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAYACTIONS_API IGameplayActionOwnerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameplayAction")
	UActorComponent* GetGameplayActionComponent() const;

};
