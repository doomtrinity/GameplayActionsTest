#include "BehaviorTree/BTService_StartGameplayAction.h"

#include "GameplayActionInterface.h"
#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"


UBTService_StartGameplayAction::UBTService_StartGameplayAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Start gameplay action";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTService_StartGameplayAction::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// start looping actions
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		for (const FGameplayTag& ActionTag : StartActionTags)
		{
			constexpr bool bTryStartImmediately = true;
			GameplayActionInterface->SetLoopingAction(ActionTag, bTryStartImmediately);
		}
	}
}

void UBTService_StartGameplayAction::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// clear looping action timers
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		for (const FGameplayTag& ActionTag : StartActionTags)
		{
			GameplayActionInterface->ClearLoopingAction(ActionTag);
		}
	}

	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

FString UBTService_StartGameplayAction::GetStaticDescription() const
{
	static const FString KeyDesc("Start gameplay action");
	return KeyDesc;
}
