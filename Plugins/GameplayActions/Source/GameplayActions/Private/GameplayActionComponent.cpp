#include "GameplayActionComponent.h"

#include "GameplayActionData.h"
#include "GameplayAction.h"
#include "GameplayActionParameters.h"
#include "TimerManager.h"
#include "GameplayActionsLog.h"
#include "GameplayActionSubsystem.h"


UGameplayActionComponent::UGameplayActionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGameplayActionComponent::BeginPlay()
{
	Super::BeginPlay();

	AddDefaultActions();
	
	if (UGameplayActionSubsystem* GameplayActionSubsystem = UGameplayActionSubsystem::Get(this))
	{
		GameplayActionSubsystem->RegisterGameplayActionInterface(this);
	}
	
}

void UGameplayActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (const TPair<FGameplayTag, UGameplayAction*>& OwnedAction : OwnedActions)
	{
		if (OwnedAction.Value && OwnedAction.Value->ShouldTickAction())
		{
			OwnedAction.Value->TickAction(DeltaTime);
		}
	}
}

void UGameplayActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearAllLoopingActions();

	StopAllActions();

	if (UGameplayActionSubsystem* GameplayActionSubsystem = UGameplayActionSubsystem::Get(this))
	{
		GameplayActionSubsystem->UnregisterGameplayActionInterface(this);
	}
	

	Super::EndPlay(EndPlayReason);
}

void UGameplayActionComponent::AddAction(const FGameplayTag& InActionTag)
{
	if (!ensure(!OwnedActions.Contains(InActionTag)))
	{
		return;
	}

	if (const UGameplayActionData* ActionData = GetOwnedActionData(InActionTag))
	{
		UGameplayAction* ActionInstance = NewObject<UGameplayAction>(GetOwner(), ActionData->GetActionClass());
		check(ActionInstance);

		if (ActionInstance != nullptr)
		{
			ActionInstance->InitAction(this, GetOwner(), InActionTag, ActionData->GetActionParameters());
			OwnedActions.Add(ActionData->GetActionTag(), ActionInstance);
		}
		
		// cache event to action mapping data
		for (const FGameplayTag& ObservedEventTag : ActionData->GetObservedEventTags())
		{
			// only 1:1 event to action mapping supported
			if (ensure(!ObservedEventToActionMapCache.Contains(ObservedEventTag)))
			{
				ObservedEventToActionMapCache.Add(ObservedEventTag, ActionData->GetActionTag());
			}
		}
		
		GAMEPLAY_ACTION_LOG(Log, TEXT("added action - actor: [%s] tag: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString());
		EnableAction(InActionTag);
		return;
	}
	ensureMsgf(false, TEXT("cannot add action [%s]"), *InActionTag.ToString());
}

void UGameplayActionComponent::RemoveAction(const FGameplayTag& InActionTag)
{
	if (!ensure(OwnedActions.Contains(InActionTag)))
	{
		return;
	}
	
	if (IsActionActive(InActionTag)) 
	{
		constexpr bool bActionCompleted = false;
		StopAction(InActionTag, bActionCompleted);
	}

	ClearLoopingAction(InActionTag);
	DisableAction(InActionTag);

	if (const UGameplayActionData* ActionData = GetOwnedActionData(InActionTag))
	{
		// clear event to action mapping data
		for (const FGameplayTag& ObservedEventTag : ActionData->GetObservedEventTags())
		{
			// only 1:1 event to action mapping supported (see AddAction)
			ObservedEventToActionMapCache.Remove(ObservedEventTag);
		}
	}
	OwnedActions.Remove(InActionTag);
	GAMEPLAY_ACTION_LOG(Log, TEXT("removed action - actor: [%s] tag: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString());
}

bool UGameplayActionComponent::OwnsAction(const FGameplayTag& InActionTag) const
{
	return OwnedActions.Contains(InActionTag);
}

TArray<FGameplayTag> UGameplayActionComponent::GetOwnedActions() const
{
	TArray<FGameplayTag> OwnedActionsTags;
	OwnedActions.GenerateKeyArray(OwnedActionsTags);
	return OwnedActionsTags;
}

void UGameplayActionComponent::EnableAction(const FGameplayTag& InActionTag)
{
	if (!ensure(OwnedActions.Contains(InActionTag)))
	{
		return;
	}

	if (UGameplayAction* Action = GetOwnedAction(InActionTag))
	{
		Action->SetActionEnabled(true);
	}

	DisabledActionsCache.RemoveSingle(InActionTag);
	GAMEPLAY_ACTION_LOG(Log, TEXT("enabled action - actor: [%s] tag: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString());
}

void UGameplayActionComponent::DisableAction(const FGameplayTag& InActionTag)
{
	if (!ensure(OwnedActions.Contains(InActionTag)))
	{
		return;
	}

	if (IsActionActive(InActionTag))
	{
		constexpr bool bActionCompleted = false;
		StopAction(InActionTag, bActionCompleted);
	}

	ClearLoopingAction(InActionTag);

	if (UGameplayAction* Action = GetOwnedAction(InActionTag))
	{
		Action->SetActionEnabled(false);
	}

	DisabledActionsCache.AddUnique(InActionTag);
	GAMEPLAY_ACTION_LOG(Log, TEXT("disabled action - actor: [%s] tag: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString());
}

bool UGameplayActionComponent::IsActionEnabled(const FGameplayTag& InActionTag) const
{
	const bool bActionEnabledCache = !DisabledActionsCache.Contains(InActionTag);
#if !UE_BUILD_SHIPPING
	if (const UGameplayAction* Action = GetOwnedAction(InActionTag))
	{
		ensure(bActionEnabledCache == Action->IsActionEnabled());
	}
#endif
	return bActionEnabledCache;
}

void UGameplayActionComponent::AddDefaultActions()
{
	for (const UGameplayActionData* ActionData : DefaultActionsData)
	{
		if (ensure(ActionData != nullptr))
		{
			// don't bother checking if the action tag to add is a leaf tag,
			// parent tags aren't supposed to be mapped (directly) to actions.
			// Also, in this simple implementation, the data is not supposed to be removed from OwnedActionsData,
			// and it is expected to be already available when calling AddAction.
			OwnedActionsData.Add(ActionData->GetActionTag(), ActionData);
			AddAction(ActionData->GetActionTag());
		}
	}
}

void UGameplayActionComponent::OnStartLoopingAction(FGameplayTag InActionTag)
{
	GAMEPLAY_ACTION_LOG(Log, TEXT("try start looping action - actor: [%s] tag: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString());
	TryStartAction(InActionTag);
	constexpr bool bTryStartImmediately = false;
	SetLoopingAction(InActionTag, bTryStartImmediately);
}

UGameplayAction* UGameplayActionComponent::GetOwnedAction(const FGameplayTag& InActionTag) const
{
	if (UGameplayAction* const* ActionPtr = OwnedActions.Find(InActionTag))
	{
		UGameplayAction* Action = *ActionPtr;
		return Action;
	}
	return nullptr;
}

const UGameplayActionData* UGameplayActionComponent::GetOwnedActionData(const FGameplayTag& InActionTag) const 
{
	if (const UGameplayActionData* const* ActionDataPtr = OwnedActionsData.Find(InActionTag))
	{
		if (const UGameplayActionData* ActionData = *ActionDataPtr)
		{
			return ActionData;
		}
	}
	return nullptr;
}

bool UGameplayActionComponent::CanStartAction(const FGameplayTag& InActionTag, TArray<FGameplayTag>& OutTagsToAbort) const
{
	OutTagsToAbort.Empty();

	if (!IsActionEnabled(InActionTag))
	{
		return false;
	}

	// check if already active
	if (IsActionActive(InActionTag))
	{
		return false;
	}

	if (const UGameplayActionData* ActionData = GetOwnedActionData(InActionTag))
	{
		// check if can abort the active action (if any)
		for (const FGameplayTag& ActiveAction : GetActiveActions())
		{
			// note: MatchesAny can be used instead, in order to match parent tags too,
			// but I think it is clearer to be explicit (unless there are lots of tags to select as abort tags)
			if (ActionData->CanAbortAllActions() || ActiveAction.MatchesAnyExact(ActionData->GetAbortActionTags()))
			{
				OutTagsToAbort.Add(ActiveAction);
			}
			else 
			{
				// cannot abort active actions
				return false;
			}
		}

		if (const UGameplayAction* Action = GetOwnedAction(InActionTag))
		{
			// check start conditions
			const bool bCanStartActionCheck = ActionData->CheckActionStartConditions(Action);
			return bCanStartActionCheck;
		}
	}
	return false;
}

bool UGameplayActionComponent::TryStartAction(const FGameplayTag& InActionTag)
{
#if !UE_BUILD_SHIPPING
	// already checked in CanStartAction, but the double checks here are for debugging purposes
	ensure(OwnsAction(InActionTag));
	ensure(IsActionEnabled(InActionTag));
	ensure(!IsActionActive(InActionTag));
#endif
	TArray<FGameplayTag> TagsToAbort;
	if (CanStartAction(InActionTag, TagsToAbort))
	{
		if (UGameplayAction* Action = GetOwnedAction(InActionTag))
		{
			// abort previous actions
			for (const FGameplayTag& TagToAbort : TagsToAbort)
			{
				constexpr bool bActionCompleted = false;
				StopAction(TagToAbort, bActionCompleted);
			}

			ActiveActionsCache.Add(InActionTag);
			GAMEPLAY_ACTION_LOG(Log, TEXT("start action - actor: [%s] tag: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString());
			// action started delegates are supposed to be triggered by the action
			// beware of the object possibly stopping the action in its start action logic, in the same frame
			Action->StartAction();
			ensureMsgf(IsActionActive(InActionTag), TEXT("action [%s] possibly stopped in start action function"), *InActionTag.ToString());
			
			return true;
		}
	}
	return false;
}

void UGameplayActionComponent::SetLoopingAction(const FGameplayTag& InActionTag, bool bTryStartImmediately)
{
	if (bTryStartImmediately && !IsActionActive(InActionTag))
	{
		TryStartAction(InActionTag);
	}
	const UGameplayActionData* ActionData = GetOwnedActionData(InActionTag);
	const UGameplayActionParameters* ActionParameters = ActionData != nullptr ? ActionData->GetActionParameters() : nullptr;
	if (ensure(ActionData != nullptr) && ensure(ActionParameters != nullptr))
	{
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::OnStartLoopingAction, InActionTag);
		const float TimeBetweenLoops = ActionParameters->GetTimeBetweenLoops(GetOwner());
		if (TimeBetweenLoops > 0.0f && GetWorld() != nullptr)
		{
			const float LoopDelay = TimeBetweenLoops;
			constexpr bool bLoop = false; // false because timer will be restarted manually
			const float FirstDelay = TimeBetweenLoops;
			FTimerHandle TimerHandle;
			if (FTimerHandle* TimerHandlePtr = LoopingActionsTimerHandles.Find(InActionTag))
			{
				// previous timer handle found, use it to reset the timer
				TimerHandle = *TimerHandlePtr;
			}
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, LoopDelay, bLoop, FirstDelay);
			if (ensure(TimerHandle.IsValid()))
			{
				LoopingActionsTimerHandles.Add(InActionTag, TimerHandle);
			}
		}
	}
}

void UGameplayActionComponent::StopAction(const FGameplayTag& InActionTag, bool bActionCompleted)
{
	if (!ensure(IsActionEnabled(InActionTag)))
	{
		return;
	}
	
	if (!ensure(IsActionActive(InActionTag)))
	{
		return;
	}
	
	if (UGameplayAction* Action = GetOwnedAction(InActionTag))
	{
		ActiveActionsCache.Remove(InActionTag);
		GAMEPLAY_ACTION_LOG(Log, TEXT("stop action - actor: [%s] tag: [%s] action completed: [%s]"), *GetNameSafe(GetOwner()), *InActionTag.ToString(), *LexToString(bActionCompleted));
		// action stopped delegates are supposed to be triggered by the action
		Action->StopAction(bActionCompleted);
		ensureMsgf(!IsActionActive(InActionTag), TEXT("action [%s] possibly started in stop action function"), *InActionTag.ToString());
	}
}

void UGameplayActionComponent::TriggerActionEvent(const FGameplayTag& InEventTag, AActor* Instigator, AActor* Trigger)
{
	const FGameplayTag* const ActionTagPtr = ObservedEventToActionMapCache.Find(InEventTag);
	// it is fine if there is no mapped action i.e. when getting here through broadcast events,
	// but the ensure helps to spot possible setup issues
	if (ensure(ActionTagPtr != nullptr))
	{
		const FGameplayTag& ActionTag = *ActionTagPtr;
		if (!IsActionEnabled(ActionTag))
		{
			return;
		}
		GAMEPLAY_ACTION_LOG(Log, TEXT("trigger action event - owner actor: [%s] action tag: [%s] event tag: [%s] instigator: [%s] trigger: [%s]")
			, *GetNameSafe(GetOwner())
			, *ActionTag.ToString()
			, *InEventTag.ToString()
			, *GetNameSafe(Instigator)
			, *GetNameSafe(Trigger)
		);

		// let the actual action know about the event to give it a chance to cache the event data,
		// before firing the delegates (which can possibly start the action)
		if (UGameplayAction* Action = GetOwnedAction(ActionTag))
		{
			Action->OnActionEvent(InEventTag, Instigator, Trigger);
		}

		FireActionEventDelegates(ActionTag, InEventTag, Instigator, Trigger);
	}
}

bool UGameplayActionComponent::IsActionActive(const FGameplayTag& InActionTag) const
{
	const bool bActionActiveCache = ActiveActionsCache.Contains(InActionTag);
#if !UE_BUILD_SHIPPING
	if (const UGameplayAction* Action = GetOwnedAction(InActionTag))
	{
		ensure(bActionActiveCache == Action->IsActionActive());
	}
#endif
	return bActionActiveCache;
}

TArray<FGameplayTag> UGameplayActionComponent::GetActiveActions() const
{
#if !UE_BUILD_SHIPPING
	TArray<FGameplayTag> ActualActiveActions;
	for (const TPair<FGameplayTag, UGameplayAction*>& OwnedAction : OwnedActions)
	{
		if (OwnedAction.Value && OwnedAction.Value->IsActionActive())
		{
			ActualActiveActions.Add(OwnedAction.Key);
		}
	}
	for (const FGameplayTag& ActiveActionCache : ActiveActionsCache)
	{
		ensure(ActualActiveActions.Contains(ActiveActionCache));
	}
#endif
	return ActiveActionsCache;
}

FDelegateHandle UGameplayActionComponent::AddActionStartedDelegate(const FGameplayActionStartedDelegate::FDelegate& InGameplayActionStartedDelegate)
{
	FDelegateHandle DelegateHandle = GameplayActionStartedDelegates.Add(InGameplayActionStartedDelegate);
	return DelegateHandle;
}

bool UGameplayActionComponent::RemoveActionStartedDelegate(FDelegateHandle DelegateHandle)
{
	if (ensure(DelegateHandle.IsValid()))
	{
		const bool bResult = GameplayActionStartedDelegates.Remove(DelegateHandle);
		return bResult;
	}
	return false;
}

void UGameplayActionComponent::AddActionStartedDynamicDelegate(FGameplayActionStartedDynamicDelegate InGameplayActionStartedDynamicDelegate) 
{
	if (ensure(!GameplayActionStartedDynamicDelegates.Contains(InGameplayActionStartedDynamicDelegate)))
	{
		GameplayActionStartedDynamicDelegates.Add(InGameplayActionStartedDynamicDelegate);
	}
}

void UGameplayActionComponent::RemoveActionStartedDynamicDelegate(FGameplayActionStartedDynamicDelegate InGameplayActionStartedDynamicDelegate)
{
	if (ensure(GameplayActionStartedDynamicDelegates.Contains(InGameplayActionStartedDynamicDelegate)))
	{
		GameplayActionStartedDynamicDelegates.Remove(InGameplayActionStartedDynamicDelegate);
	}
}

void UGameplayActionComponent::FireActionStartedDelegates(const FGameplayTag& InActionTag)
{
	// action should be active here, ensure if not, but broadcast the event anyway
	ensure(ActiveActionsCache.Contains(InActionTag));
	
	GameplayActionStartedDelegates.Broadcast(GetOwner(), InActionTag);

	// copy the array of the dynamic delegates, just in case it is modified by one of the callbacks
	TArray<FGameplayActionStartedDynamicDelegate> GameplayActionStartedDynamicDelegatesCopy = GameplayActionStartedDynamicDelegates;
	for (int32 i = 0; i < GameplayActionStartedDynamicDelegatesCopy.Num(); i++)
	{
		GameplayActionStartedDynamicDelegatesCopy[i].ExecuteIfBound(GetOwner(), InActionTag);
	}
}

FDelegateHandle UGameplayActionComponent::AddActionStoppedDelegate(const FGameplayActionStoppedDelegate::FDelegate& InGameplayActionStoppedDelegate)
{
	FDelegateHandle DelegateHandle = GameplayActionStoppedDelegates.Add(InGameplayActionStoppedDelegate);
	return DelegateHandle;
}

bool UGameplayActionComponent::RemoveActionStoppedDelegate(FDelegateHandle DelegateHandle)
{
	if (ensure(DelegateHandle.IsValid()))
	{
		const bool bResult = GameplayActionStoppedDelegates.Remove(DelegateHandle);
		return bResult;
	}
	return false;
}

void UGameplayActionComponent::AddActionStoppedDynamicDelegate(FGameplayActionStoppedDynamicDelegate InGameplayActionStoppedDynamicDelegate)
{
	if (ensure(!GameplayActionStoppedDynamicDelegates.Contains(InGameplayActionStoppedDynamicDelegate)))
	{
		GameplayActionStoppedDynamicDelegates.Add(InGameplayActionStoppedDynamicDelegate);
	}
}

void UGameplayActionComponent::RemoveActionStoppedDynamicDelegate(FGameplayActionStoppedDynamicDelegate InGameplayActionStoppedDynamicDelegate)
{
	if (ensure(GameplayActionStoppedDynamicDelegates.Contains(InGameplayActionStoppedDynamicDelegate)))
	{
		GameplayActionStoppedDynamicDelegates.Remove(InGameplayActionStoppedDynamicDelegate);
	}
}

void UGameplayActionComponent::FireActionStoppedDelegates(const FGameplayTag& InActionTag, bool bInActionAborted)
{
	// action is not expected to be active here, ensure if still active, but broadcast the event anyway
	ensure(!ActiveActionsCache.Contains(InActionTag));

	GameplayActionStoppedDelegates.Broadcast(GetOwner(), InActionTag, bInActionAborted);

	// copy the array of the dynamic delegates, just in case it is modified by one of the callbacks
	TArray<FGameplayActionStoppedDynamicDelegate> GameplayActionStoppedDynamicDelegatesCopy = GameplayActionStoppedDynamicDelegates;
	for (int32 i = 0; i < GameplayActionStoppedDynamicDelegatesCopy.Num(); i++)
	{
		GameplayActionStoppedDynamicDelegatesCopy[i].ExecuteIfBound(GetOwner(), InActionTag, bInActionAborted);
	}
}

FDelegateHandle UGameplayActionComponent::AddActionEventDelegate(const FGameplayActionEventDelegate::FDelegate& InGameplayActionEventDelegate)
{
	FDelegateHandle DelegateHandle = GameplayActionEventDelegates.Add(InGameplayActionEventDelegate);
	return DelegateHandle;
}

bool UGameplayActionComponent::RemoveActionEventDelegate(FDelegateHandle DelegateHandle)
{
	if (ensure(DelegateHandle.IsValid()))
	{
		const bool bResult = GameplayActionEventDelegates.Remove(DelegateHandle);
		return bResult;
	}
	return false;
}

void UGameplayActionComponent::AddActionEventDynamicDelegate(FGameplayActionEventDynamicDelegate InGameplayActionEventDynamicDelegate)
{
	if (ensure(!GameplayActionEventDynamicDelegates.Contains(InGameplayActionEventDynamicDelegate)))
	{
		GameplayActionEventDynamicDelegates.Add(InGameplayActionEventDynamicDelegate);
	}
}

void UGameplayActionComponent::RemoveActionEventDynamicDelegate(FGameplayActionEventDynamicDelegate InGameplayActionEventDynamicDelegate)
{
	if (ensure(GameplayActionEventDynamicDelegates.Contains(InGameplayActionEventDynamicDelegate)))
	{
		GameplayActionEventDynamicDelegates.Remove(InGameplayActionEventDynamicDelegate);
	}
}

void UGameplayActionComponent::FireActionEventDelegates(const FGameplayTag& InActionTag, const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger)
{
	GameplayActionEventDelegates.Broadcast(GetOwner(), InActionTag, InEventTag, InInstigator, InTrigger);

	// copy the array of the dynamic delegates, just in case it is modified by one of the callbacks
	TArray<FGameplayActionEventDynamicDelegate> GameplayActionEventDynamicDelegatesCopy = GameplayActionEventDynamicDelegates;
	for (int32 i = 0; i < GameplayActionEventDynamicDelegatesCopy.Num(); i++)
	{
		GameplayActionEventDynamicDelegatesCopy[i].ExecuteIfBound(GetOwner(), InActionTag, InEventTag, InInstigator, InTrigger);
	}
}

void UGameplayActionComponent::ClearLoopingAction(const FGameplayTag& InActionTag)
{
	if (FTimerHandle* TimerHandlePtr = LoopingActionsTimerHandles.Find(InActionTag))
	{
		if (GetWorld() != nullptr)
		{
			FTimerHandle TimerHandle = *TimerHandlePtr;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			LoopingActionsTimerHandles.Remove(InActionTag);
		}
	}
}

void UGameplayActionComponent::ClearAllLoopingActions() 
{
	TArray<FGameplayTag> LoopingActionTags;
	LoopingActionsTimerHandles.GenerateKeyArray(LoopingActionTags);
	for (const FGameplayTag& LoopingActionTag : LoopingActionTags)
	{
		ClearLoopingAction(LoopingActionTag);
	}
}

void UGameplayActionComponent::StopAllActions()
{
	for (const FGameplayTag& ActiveAction : GetActiveActions())
	{
		constexpr bool bActionCompleted = false;
		StopAction(ActiveAction, bActionCompleted);
	}
}
