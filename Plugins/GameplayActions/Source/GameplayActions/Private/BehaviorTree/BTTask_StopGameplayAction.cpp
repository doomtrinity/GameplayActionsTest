#include "BehaviorTree/BTTask_StopGameplayAction.h"

#include "GameplayActionInterface.h"
#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"


UBTTask_StopGameplayAction::UBTTask_StopGameplayAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Stop gameplay action";
}

EBTNodeResult::Type UBTTask_StopGameplayAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		for (const FGameplayTag& ActionTag : GameplayActionInterface->GetActiveActions())
		{
			if (bStopAllActions || ActionTag.MatchesAnyExact(StopActionTags)) 
			{
				constexpr bool bActionCompleted = false;
				GameplayActionInterface->StopAction(ActionTag, bActionCompleted);
			}
		}
	}
	return EBTNodeResult::Succeeded;
}

FString UBTTask_StopGameplayAction::GetStaticDescription() const
{
	static const FString KeyDesc("Stop gameplay action");
	return KeyDesc;
}
