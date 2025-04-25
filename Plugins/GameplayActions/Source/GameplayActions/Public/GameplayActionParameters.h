#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayActionParameters.generated.h"

class AActor;


UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Const, hideCategories = Object, BlueprintType, Blueprintable, meta = (ShowWorldContextPin))
class GAMEPLAYACTIONS_API UGameplayActionParameters : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayAction")
	float GetTimeBetweenLoops(const AActor* TargetActor) const;

protected:

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	float TimeBetweenLoops = -1.f;

};
