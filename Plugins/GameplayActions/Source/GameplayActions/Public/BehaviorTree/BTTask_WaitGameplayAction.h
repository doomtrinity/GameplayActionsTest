#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"

#include "BTTask_WaitGameplayAction.generated.h"

class AActor;
class IGameplayActionInterface;


struct FBTWaitGameplayActionMemory
{
	FDelegateHandle ActionStoppedDelegateHandle;
};


UCLASS()
class GAMEPLAYACTIONS_API UBTTask_WaitGameplayAction : public UBTTaskNode
{
	GENERATED_BODY()

public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

protected:

	UBTTask_WaitGameplayAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTWaitGameplayActionMemory); }
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	FGameplayTagContainer WaitActionTags;

private:

	void OnActionStopped(UBehaviorTreeComponent& OwnerComp, AActor* InActionOwner, const FGameplayTag& InActionTag, bool bInActionAborted) const;
	bool HasActiveActions(const IGameplayActionInterface* InGameplayActionInterface) const;
};
