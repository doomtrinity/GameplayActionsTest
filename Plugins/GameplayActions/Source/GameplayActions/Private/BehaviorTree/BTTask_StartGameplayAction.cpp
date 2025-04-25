#include "BehaviorTree/BTTask_StartGameplayAction.h"

#include "GameplayActionInterface.h"
#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"


UBTTask_StartGameplayAction::UBTTask_StartGameplayAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Start single gameplay action";
}

EBTNodeResult::Type UBTTask_StartGameplayAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (IGameplayActionInterface* GameplayActionInterface = UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(OwnerComp))
	{
		if (GameplayActionInterface->TryStartAction(StartActionTag)) 
		{
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

FString UBTTask_StartGameplayAction::GetStaticDescription() const
{
	static const FString KeyDesc("Start single gameplay action");
	return KeyDesc;
}
