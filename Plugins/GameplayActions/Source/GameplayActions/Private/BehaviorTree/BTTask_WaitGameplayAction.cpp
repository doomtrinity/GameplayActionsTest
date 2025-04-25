#include "BehaviorTree/BTTask_WaitGameplayAction.h"

#include "GameFramework/Actor.h"
#include "GameplayActionInterface.h"
#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"


UBTTask_WaitGameplayAction::UBTTask_WaitGameplayAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Wait gameplay action";

	bNotifyTaskFinished = true;
}

bool UBTTask_WaitGameplayAction::HasActiveActions(const IGameplayActionInterface* InGameplayActionInterface) const
{
	if (InGameplayActionInterface)
	{
		for (const FGameplayTag& ActionTag : InGameplayActionInterface->GetActiveActions())
		{
			if (ActionTag.MatchesAnyExact(WaitActionTags)) 
			{
				return true;
			}
		}
	}
	return false;
}

void UBTTask_WaitGameplayAction::OnActionStopped(UBehaviorTreeComponent& OwnerComp, AActor* InActionOwner, const FGameplayTag& InActionTag, bool bInActionAborted) const
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		if (HasActiveActions(GameplayActionInterface))
		{
			return;
		}
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_WaitGameplayAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		if (!HasActiveActions(GameplayActionInterface))
		{
			return EBTNodeResult::Succeeded;
		}

		TWeakObjectPtr<const UBTTask_WaitGameplayAction> WeakThis(this);
		TWeakObjectPtr<UBehaviorTreeComponent> WeakBehaviorComp(&OwnerComp);
		FGameplayActionStoppedDelegate::FDelegate ActionStoppedDelegate = FGameplayActionStoppedDelegate::FDelegate::CreateLambda(
			[WeakThis, WeakBehaviorComp]
			(AActor* InActionOwner, const FGameplayTag& InActionTag, bool bInActionAborted)
			{
				if (WeakThis.IsValid() && WeakBehaviorComp.IsValid())
				{
					WeakThis->OnActionStopped(*WeakBehaviorComp, InActionOwner, InActionTag, bInActionAborted);
				}
			});

		FDelegateHandle ActionStoppedDelegateHandle = GameplayActionInterface->AddActionStoppedDelegate(ActionStoppedDelegate);
		FBTWaitGameplayActionMemory* MyMemory = CastInstanceNodeMemory<FBTWaitGameplayActionMemory>(NodeMemory);
		check(MyMemory);
		if (MyMemory != nullptr && ensure(ActionStoppedDelegateHandle.IsValid()))
		{
			MyMemory->ActionStoppedDelegateHandle = ActionStoppedDelegateHandle;
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Succeeded;
}

void UBTTask_WaitGameplayAction::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		FBTWaitGameplayActionMemory* MyMemory = CastInstanceNodeMemory<FBTWaitGameplayActionMemory>(NodeMemory);
		check(MyMemory);
		if (MyMemory != nullptr && MyMemory->ActionStoppedDelegateHandle.IsValid())
		{
			GameplayActionInterface->RemoveActionStoppedDelegate(MyMemory->ActionStoppedDelegateHandle);
			MyMemory->ActionStoppedDelegateHandle.Reset();
		}
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

FString UBTTask_WaitGameplayAction::GetStaticDescription() const
{
	static const FString KeyDesc("Wait gameplay action");
	return KeyDesc;
}
