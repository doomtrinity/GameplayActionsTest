#include "BehaviorTree/BTDecorator_CheckActiveGameplayAction.h"

#include "GameFramework/Actor.h"
#include "GameplayActionInterface.h"
#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"


UBTDecorator_CheckActiveGameplayAction::UBTDecorator_CheckActiveGameplayAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Check active gameplay actions";

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	FlowAbortMode = EBTFlowAbortMode::None;
	bAllowAbortNone = true;
	bAllowAbortLowerPri = true;
	bAllowAbortChildNodes = true;
}

bool UBTDecorator_CheckActiveGameplayAction::HasActiveActions(const IGameplayActionInterface* InGameplayActionInterface) const
{
	if (InGameplayActionInterface)
	{
		for (const FGameplayTag& ActionTag : InGameplayActionInterface->GetActiveActions())
		{
			if (ActionTag.MatchesAnyExact(ActiveActionTags))
			{
				return true;
			}
		}
	}
	return false;
}

bool UBTDecorator_CheckActiveGameplayAction::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		if (HasActiveActions(GameplayActionInterface))
		{
			return true;
		}
	}
	return false;
}

void UBTDecorator_CheckActiveGameplayAction::OnActionStarted(UBehaviorTreeComponent& OwnerComp, AActor* InActionOwner, const FGameplayTag& InActionTag) const
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		if (HasActiveActions(GameplayActionInterface))
		{
			const EBTDecoratorAbortRequest RequestMode = EBTDecoratorAbortRequest::ConditionPassing;
			ConditionalFlowAbort(OwnerComp, RequestMode);
		}
	}
}

void UBTDecorator_CheckActiveGameplayAction::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		TWeakObjectPtr<const UBTDecorator_CheckActiveGameplayAction> WeakThis(this);
		TWeakObjectPtr<UBehaviorTreeComponent> WeakBehaviorComp(&OwnerComp);
		FGameplayActionStartedDelegate::FDelegate ActionStartedDelegate = FGameplayActionStartedDelegate::FDelegate::CreateLambda(
			[WeakThis, WeakBehaviorComp]
			(AActor* InActionOwner, const FGameplayTag& InActionTag)
			{
				if (WeakThis.IsValid() && WeakBehaviorComp.IsValid())
				{
					WeakThis->OnActionStarted(*WeakBehaviorComp, InActionOwner, InActionTag);
				}
			});

		FDelegateHandle ActionStartedDelegateHandle = GameplayActionInterface->AddActionStartedDelegate(ActionStartedDelegate);
		FBTCheckActiveGameplayActionMemory* MyMemory = CastInstanceNodeMemory<FBTCheckActiveGameplayActionMemory>(NodeMemory);
		check(MyMemory);
		if (MyMemory != nullptr && ensure(ActionStartedDelegateHandle.IsValid()))
		{
			MyMemory->ActionStartedDelegateHandle = ActionStartedDelegateHandle;
		}
	}
}

void UBTDecorator_CheckActiveGameplayAction::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		FBTCheckActiveGameplayActionMemory* MyMemory = CastInstanceNodeMemory<FBTCheckActiveGameplayActionMemory>(NodeMemory);
		check(MyMemory);
		if (MyMemory != nullptr && ensure(MyMemory->ActionStartedDelegateHandle.IsValid()))
		{
			GameplayActionInterface->RemoveActionStartedDelegate(MyMemory->ActionStartedDelegateHandle);
			MyMemory->ActionStartedDelegateHandle.Reset();
		}
	}

	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

FString UBTDecorator_CheckActiveGameplayAction::GetStaticDescription() const
{
	static const FString KeyDesc("Check active gameplay actions");
	return KeyDesc;
}
