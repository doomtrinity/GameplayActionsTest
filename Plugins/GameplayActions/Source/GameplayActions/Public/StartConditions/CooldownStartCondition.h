#pragma once

#include "GameplayActionStartCondition.h"

#include "CooldownStartCondition.generated.h"

class UGameplayAction;


UCLASS()
class GAMEPLAYACTIONS_API UCooldownStartCondition : public UGameplayActionStartCondition
{
	GENERATED_BODY()

protected:

	virtual bool IsConditionSatisfied_Implementation(const UGameplayAction* InGameplayAction) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayAction", meta = (ClampMin = "0", UIMin = "0"))
	float DefaultCooldownTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	bool bIgnoreGlobalActionStartTime = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayAction", meta = (EditCondition = "!bIgnoreGlobalActionStartTime", ClampMin = "0", UIMin = "0"))
	float DefaultGlobalCooldownTime = 0.32f;

};
