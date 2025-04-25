#include "GameplayActionSubsystem.h"

#include "GameplayActionInterface.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"


void UGameplayActionSubsystem::RegisterGameplayActionInterface(IGameplayActionInterface* InGameplayActionInterface)
{
#if !UE_BUILD_SHIPPING
	if (ensure(InGameplayActionInterface != nullptr)) 
	{
		ensure(!GameplayActionInterfaces.Contains(InGameplayActionInterface));
	}
#endif
	if (InGameplayActionInterface != nullptr)
	{
		GameplayActionInterfaces.AddUnique(InGameplayActionInterface);
	}
}

void UGameplayActionSubsystem::UnregisterGameplayActionInterface(IGameplayActionInterface* InGameplayActionInterface) 
{
#if !UE_BUILD_SHIPPING
	if (ensure(InGameplayActionInterface != nullptr))
	{
		ensure(GameplayActionInterfaces.Contains(InGameplayActionInterface));
	}
#endif
	GameplayActionInterfaces.Remove(InGameplayActionInterface);
}

void UGameplayActionSubsystem::BroadcastGameplayActionEvent(const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger) 
{
	for (TWeakInterfacePtr<IGameplayActionInterface> GameplayActionInterface : GameplayActionInterfaces) 
	{
		if (GameplayActionInterface.IsValid()) 
		{
			GameplayActionInterface->TriggerActionEvent(InEventTag, InInstigator, InTrigger);
		}
	}
}

float UGameplayActionSubsystem::GetLastGameplayActionStartTime(const FGameplayTag& InActionTag) const
{
	if (const float* LastStartTime = LastGameplayActionsStartTime.Find(InActionTag)) 
	{
		return *LastStartTime;
	}
	return -1.0f;
}

void UGameplayActionSubsystem::SetLastGameplayActionStartTime(const FGameplayTag& InActionTag, float StartTime)
{
	LastGameplayActionsStartTime.Add(InActionTag, StartTime);
}

UGameplayActionSubsystem* UGameplayActionSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* ContextWorld = WorldContextObject != nullptr ? WorldContextObject->GetWorld() : nullptr;
	if (ContextWorld)
	{
		return ContextWorld->GetSubsystem<UGameplayActionSubsystem>();
	}
	return nullptr;
}
