#include "GameplayActionsCheatManager.h"

#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "GameplayActionsLog.h"
#include "GameplayActionInterface.h"
#include "GameplayActionOwnerInterface.h"


void UGameplayActionsCheatManager::OnActionStarted(AActor* InActionOwner, const FGameplayTag& InActionTag)
{
	GAMEPLAY_ACTION_LOG(Warning, TEXT("action started delegate called - actor: [%s] tag: [%s]"), *GetNameSafe(InActionOwner), *InActionTag.ToString());
}

void UGameplayActionsCheatManager::OnActionStopped(AActor* InActionOwner, const FGameplayTag& InActionTag, bool bInActionAborted)
{
	GAMEPLAY_ACTION_LOG(Warning, TEXT("action stopped delegate called - actor: [%s] tag: [%s] action aborted:[%s]"), *GetNameSafe(InActionOwner), *InActionTag.ToString(), *LexToString(bInActionAborted));
}

void UGameplayActionsCheatManager::OnActionEvent(AActor* InActionOwner, const FGameplayTag& InActionTag, const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger)
{
	GAMEPLAY_ACTION_LOG(Log, TEXT("action event delegate called - owner actor: [%s] action tag: [%s] event tag: [%s] instigator: [%s] trigger: [%s]")
		, *GetNameSafe(InActionOwner)
		, *InActionTag.ToString()
		, *InEventTag.ToString()
		, *GetNameSafe(InInstigator)
		, *GetNameSafe(InTrigger)
	);
}

IGameplayActionInterface* UGameplayActionsCheatManager::FindFirstGameplayActionInterface() const
{
	UActorComponent* GameplayActionComponent = FindFirstGameplayActionComponent();
	IGameplayActionInterface* GameplayActionInterface = Cast<IGameplayActionInterface>(GameplayActionComponent);
	return GameplayActionInterface;
}

UActorComponent* UGameplayActionsCheatManager::FindFirstGameplayActionComponent() const
{
	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It)
	{
		if (APawn* Pawn = *It)
		{
			// note: AActor::GetComponentsByInterface(UGameplayActionInterface::StaticClass()) would also do the trick,
			// but the owner interface has been created to be explicit about the way to get it
			// (and to avoid having to get and check the first component of the array).
			// Note: using the IMyInterface::Execute_MyFunction(MyUObject) way to call the interface function,
			// as the interface function can be implemented in blueprint 
			// (Cast<IMyInterface> and then calling the function on it would only work for plain C++ functions).
			if (Pawn->Implements<UGameplayActionOwnerInterface>()) 
			{
				UActorComponent* GameplayActionComponent = IGameplayActionOwnerInterface::Execute_GetGameplayActionComponent(Pawn);
				return GameplayActionComponent;
			}
		}
	}
	return nullptr;
}

void UGameplayActionsCheatManager::GameplayActions_AddActionStartedDelegate()
{
	if (IGameplayActionInterface* GameplayActionInterface = FindFirstGameplayActionInterface())
	{
		FGameplayActionStartedDelegate::FDelegate ActionStartedDelegate = FGameplayActionStartedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnActionStarted);
		ActionStartedDelegateHandle = GameplayActionInterface->AddActionStartedDelegate(ActionStartedDelegate);
	}
}

void UGameplayActionsCheatManager::GameplayActions_RemoveActionStartedDelegate()
{
	if (IGameplayActionInterface* GameplayActionInterface = FindFirstGameplayActionInterface())
	{
		GameplayActionInterface->RemoveActionStartedDelegate(ActionStartedDelegateHandle);
	}
}

void UGameplayActionsCheatManager::GameplayActions_AddActionStoppedDelegate()
{
	if (IGameplayActionInterface* GameplayActionInterface = FindFirstGameplayActionInterface())
	{
		FGameplayActionStoppedDelegate::FDelegate ActionStoppedDelegate = FGameplayActionStoppedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnActionStopped);
		ActionStoppedDelegateHandle = GameplayActionInterface->AddActionStoppedDelegate(ActionStoppedDelegate);
	}
}

void UGameplayActionsCheatManager::GameplayActions_RemoveActionStoppedDelegate()
{
	if (IGameplayActionInterface* GameplayActionInterface = FindFirstGameplayActionInterface())
	{
		GameplayActionInterface->RemoveActionStoppedDelegate(ActionStoppedDelegateHandle);
	}
}

void UGameplayActionsCheatManager::GameplayActions_AddActionEventDelegate()
{
	if (IGameplayActionInterface* GameplayActionInterface = FindFirstGameplayActionInterface())
	{
		FGameplayActionEventDelegate::FDelegate ActionEventDelegate = FGameplayActionEventDelegate::FDelegate::CreateUObject(this, &ThisClass::OnActionEvent);
		ActionEventDelegateHandle = GameplayActionInterface->AddActionEventDelegate(ActionEventDelegate);
	}
}

void UGameplayActionsCheatManager::GameplayActions_RemoveActionEventDelegate()
{
	if (IGameplayActionInterface* GameplayActionInterface = FindFirstGameplayActionInterface())
	{
		GameplayActionInterface->RemoveActionEventDelegate(ActionEventDelegateHandle);
	}
}
