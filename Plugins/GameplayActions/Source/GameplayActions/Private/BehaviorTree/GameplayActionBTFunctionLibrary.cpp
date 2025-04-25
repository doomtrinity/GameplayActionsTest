#include "BehaviorTree/GameplayActionBTFunctionLibrary.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "GameplayActionInterface.h"


IGameplayActionInterface* UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(AActor* ActionOwner)
{
	IGameplayActionInterface* GameplayActionInterface = nullptr;
	if (ActionOwner)
	{
		TArray<UActorComponent*> ActionOwnerComponents = ActionOwner->GetComponentsByInterface(UGameplayActionInterface::StaticClass());
		if (ActionOwnerComponents.Num() > 0)
		{
			GameplayActionInterface = Cast<IGameplayActionInterface>(ActionOwnerComponents[0]);
		}
	}
	return GameplayActionInterface;
}

IGameplayActionInterface* UGameplayActionBTFunctionLibrary::GetGameplayActionInterface(UBehaviorTreeComponent& OwnerComp)
{
	AActor* GameplayActionOwner = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	IGameplayActionInterface* GameplayActionInterface = GetGameplayActionInterface(GameplayActionOwner);
	return GameplayActionInterface;
}
