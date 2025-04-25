#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"

#include "GameplayActionInterface.generated.h"

class AActor;

// C++ delegates
DECLARE_MULTICAST_DELEGATE_TwoParams(FGameplayActionStartedDelegate, AActor* /*InActionOwner*/, const FGameplayTag& /*InActionTag*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FGameplayActionStoppedDelegate, AActor* /*InActionOwner*/, const FGameplayTag& /*InActionTag*/, bool /*bInActionAborted*/);
DECLARE_MULTICAST_DELEGATE_FiveParams(FGameplayActionEventDelegate, AActor* /*InActionOwner*/, const FGameplayTag& /*InActionTag*/, const FGameplayTag& /*InEventTag*/, AActor* /*InInstigator*/, AActor* /*InTrigger*/);

// blueprint delegates
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGameplayActionStartedDynamicDelegate, AActor*, InActionOwner, const FGameplayTag&, InActionTag);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FGameplayActionStoppedDynamicDelegate, AActor*, InActionOwner, const FGameplayTag&, InActionTag, bool, bInActionAborted);
DECLARE_DYNAMIC_DELEGATE_FiveParams(FGameplayActionEventDynamicDelegate, AActor*, InActionOwner, const FGameplayTag&, InActionTag, const FGameplayTag&, InEventTag, AActor*, InInstigator, AActor*, InTrigger);


// this class does not need to be modified
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UGameplayActionInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAYACTIONS_API IGameplayActionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual bool CanStartAction(const FGameplayTag& InActionTag, TArray<FGameplayTag>& OutTagsToAbort) const PURE_VIRTUAL(IGameplayActionInterface::CanStartAction, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual bool TryStartAction(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::TryStartAction, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void SetLoopingAction(const FGameplayTag& InActionTag, bool bTryStartImmediately) PURE_VIRTUAL(IGameplayActionInterface::SetLoopingAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void StopAction(const FGameplayTag& InActionTag, bool bActionCompleted) PURE_VIRTUAL(IGameplayActionInterface::StopAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void TriggerActionEvent(const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger) PURE_VIRTUAL(IGameplayActionInterface::TriggerActionEvent, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual bool IsActionActive(const FGameplayTag& InActionTag) const PURE_VIRTUAL(IGameplayActionInterface::IsActionActive, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual TArray<FGameplayTag> GetActiveActions() const PURE_VIRTUAL(IGameplayActionInterface::GetActiveActions, return TArray<FGameplayTag>(););

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void ClearLoopingAction(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::ClearLoopingAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void AddAction(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::AddAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void RemoveAction(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::RemoveAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual bool OwnsAction(const FGameplayTag& InActionTag) const PURE_VIRTUAL(IGameplayActionInterface::OwnsAction, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void EnableAction(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::EnableAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void DisableAction(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::DisableAction, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual bool IsActionEnabled(const FGameplayTag& InActionTag) const PURE_VIRTUAL(IGameplayActionInterface::IsActionEnabled, return false;);


	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual TArray<FGameplayTag> GetOwnedActions() const PURE_VIRTUAL(IGameplayActionInterface::GetOwnedActions, return TArray<FGameplayTag>(););

	// action started delegates
	virtual FDelegateHandle AddActionStartedDelegate(const FGameplayActionStartedDelegate::FDelegate& InGameplayActionStartedDelegate) PURE_VIRTUAL(IGameplayActionInterface::AddActionStartedDelegate, return FDelegateHandle(););
	virtual bool RemoveActionStartedDelegate(FDelegateHandle DelegateHandle) PURE_VIRTUAL(IGameplayActionInterface::RemoveActionStartedDelegate, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void AddActionStartedDynamicDelegate(FGameplayActionStartedDynamicDelegate InGameplayActionStartedDynamicDelegate) PURE_VIRTUAL(IGameplayActionInterface::AddActionStartedDynamicDelegate, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void RemoveActionStartedDynamicDelegate(FGameplayActionStartedDynamicDelegate InGameplayActionStartedDynamicDelegate) PURE_VIRTUAL(IGameplayActionInterface::RemoveActionStartedDynamicDelegate, );

	// action stopped delegates
	virtual FDelegateHandle AddActionStoppedDelegate(const FGameplayActionStoppedDelegate::FDelegate& InGameplayActionStoppedDelegate) PURE_VIRTUAL(IGameplayActionInterface::AddActionStoppedDelegate, return FDelegateHandle(););
	virtual bool RemoveActionStoppedDelegate(FDelegateHandle DelegateHandle) PURE_VIRTUAL(IGameplayActionInterface::RemoveActionStoppedDelegate, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void AddActionStoppedDynamicDelegate(FGameplayActionStoppedDynamicDelegate InGameplayActionStoppedDynamicDelegate) PURE_VIRTUAL(IGameplayActionInterface::AddActionStoppedDynamicDelegate, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void RemoveActionStoppedDynamicDelegate(FGameplayActionStoppedDynamicDelegate InGameplayActionStoppedDynamicDelegate) PURE_VIRTUAL(IGameplayActionInterface::RemoveActionStoppedDynamicDelegate, );

	// action event triggered delegates
	virtual FDelegateHandle AddActionEventDelegate(const FGameplayActionEventDelegate::FDelegate& InGameplayActionEventDelegate) PURE_VIRTUAL(IGameplayActionInterface::AddActionEventDelegate, return FDelegateHandle(););
	virtual bool RemoveActionEventDelegate(FDelegateHandle DelegateHandle) PURE_VIRTUAL(IGameplayActionInterface::RemoveActionEventDelegate, return false;);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void AddActionEventDynamicDelegate(FGameplayActionEventDynamicDelegate InGameplayActionEventDynamicDelegate) PURE_VIRTUAL(IGameplayActionInterface::AddActionEventDynamicDelegate, );

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	virtual void RemoveActionEventDynamicDelegate(FGameplayActionEventDynamicDelegate InGameplayActionEventDynamicDelegate) PURE_VIRTUAL(IGameplayActionInterface::RemoveActionEventDynamicDelegate, );

	virtual void FireActionStartedDelegates(const FGameplayTag& InActionTag) PURE_VIRTUAL(IGameplayActionInterface::FireActionStartedDelegates, );
	virtual void FireActionStoppedDelegates(const FGameplayTag& InActionTag, bool bInActionAborted) PURE_VIRTUAL(IGameplayActionInterface::FireActionStoppedDelegates, );

};
