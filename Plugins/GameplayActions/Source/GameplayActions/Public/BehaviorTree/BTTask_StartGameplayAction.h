#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"

#include "BTTask_StartGameplayAction.generated.h"


UCLASS()
class GAMEPLAYACTIONS_API UBTTask_StartGameplayAction : public UBTTaskNode
{
	GENERATED_BODY()

public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

protected:

	UBTTask_StartGameplayAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	FGameplayTag StartActionTag;

};
