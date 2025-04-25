#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "GameplayActionData.generated.h"

class UGameplayAction;
class UGameplayActionParameters;
class UGameplayActionStartCondition;


UCLASS(Const)
class GAMEPLAYACTIONS_API UGameplayActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	FGameplayTag GetActionTag() const;

	TSubclassOf<UGameplayAction> GetActionClass() const;

	const UGameplayActionParameters* GetActionParameters() const;

	bool CheckActionStartConditions(const UGameplayAction* InGameplayAction) const;

	bool CanAbortAllActions() const;

	const FGameplayTagContainer& GetAbortActionTags() const;

	const FGameplayTagContainer& GetObservedEventTags() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAction")
	FGameplayTag ActionTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAction")
	TSubclassOf<UGameplayAction> ActionClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "GameplayAction")
	UGameplayActionParameters* ActionParameters = nullptr;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "GameplayAction")
	TArray<UGameplayActionStartCondition*> ActionStartConditions;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAction")
	bool bAbortAllActions = false;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAction", meta = (EditCondition = "!bAbortAllActions"))
	FGameplayTagContainer AbortActionTags;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAction")
	FGameplayTagContainer ObservedEventTags;

};
