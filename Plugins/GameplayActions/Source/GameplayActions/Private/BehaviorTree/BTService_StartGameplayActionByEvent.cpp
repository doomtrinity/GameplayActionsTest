#include "BehaviorTree/BTService_StartGameplayActionByEvent.h"

#include "GameFramework/Actor.h"
#include "GameplayActionInterface.h"
#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"


UBTService_StartGameplayActionByEvent::UBTService_StartGameplayActionByEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Start gameplay action by event";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTService_StartGameplayActionByEvent::OnActionEvent(AActor* InActionOwner, const FGameplayTag& InActionTag, const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger)
{
	if (InEventTag.MatchesAnyExact(ObservedActionEventTags))
	{
		// start action by event
		if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(InActionOwner))
		{
			if (!GameplayActionInterface->IsActionActive(InActionTag))
			{
				GameplayActionInterface->TryStartAction(InActionTag);
			}
		}
	}
}

void UBTService_StartGameplayActionByEvent::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// add action event delegates
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		FGameplayActionEventDelegate::FDelegate ActionEventDelegate = FGameplayActionEventDelegate::FDelegate::CreateUObject(this, &ThisClass::OnActionEvent);
		FDelegateHandle ActionEventDelegateHandle = GameplayActionInterface->AddActionEventDelegate(ActionEventDelegate);
		FBTStartGameplayActionByEventMemory* MyMemory = CastInstanceNodeMemory<FBTStartGameplayActionByEventMemory>(NodeMemory);
		check(MyMemory);
		if (MyMemory != nullptr && ensure(ActionEventDelegateHandle.IsValid()))
		{
			MyMemory->ActionEventDelegateHandle = ActionEventDelegateHandle;
		}
	}
}

void UBTService_StartGameplayActionByEvent::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		// remove action event delegates
		FBTStartGameplayActionByEventMemory* MyMemory = CastInstanceNodeMemory<FBTStartGameplayActionByEventMemory>(NodeMemory);
		check(MyMemory);
		if (MyMemory != nullptr && ensure(MyMemory->ActionEventDelegateHandle.IsValid()))
		{
			GameplayActionInterface->RemoveActionEventDelegate(MyMemory->ActionEventDelegateHandle);
			MyMemory->ActionEventDelegateHandle.Reset();
		}
	}

	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

FString UBTService_StartGameplayActionByEvent::GetStaticDescription() const
{
	static const FString KeyDesc("Start gameplay action by event");
	return KeyDesc;
}
