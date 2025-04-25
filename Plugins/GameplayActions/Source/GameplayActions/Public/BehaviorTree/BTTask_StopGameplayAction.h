#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"

#include "BTTask_StopGameplayAction.generated.h"


UCLASS()
class GAMEPLAYACTIONS_API UBTTask_StopGameplayAction : public UBTTaskNode
{
	GENERATED_BODY()

public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

protected:

	UBTTask_StopGameplayAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	bool bStopAllActions = false;

	UPROPERTY(EditAnywhere, Category = "GameplayAction", meta = (EditCondition = "!bStopAllActions"))
	FGameplayTagContainer StopActionTags;

};
