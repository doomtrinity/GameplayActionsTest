#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"

#include "BTService_StartGameplayAction.generated.h"


UCLASS(hidecategories = (Service))
class GAMEPLAYACTIONS_API UBTService_StartGameplayAction : public UBTService
{
	GENERATED_BODY()

protected:

	UBTService_StartGameplayAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	FGameplayTagContainer StartActionTags;

};
