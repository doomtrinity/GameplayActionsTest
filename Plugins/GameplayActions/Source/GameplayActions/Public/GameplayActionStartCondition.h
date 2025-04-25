#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayActionStartCondition.generated.h"

class UGameplayAction;


UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Const, hideCategories = Object, BlueprintType, Blueprintable, meta = (ShowWorldContextPin))
class GAMEPLAYACTIONS_API UGameplayActionStartCondition : public UObject
{
	GENERATED_BODY()

public:

	bool CheckCondition(const UGameplayAction* InGameplayAction) const;

protected:

	UFUNCTION(BlueprintNativeEvent)
	bool IsConditionSatisfied(const UGameplayAction* InGameplayAction) const;

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	bool bIgnoreCheck = false;

};
