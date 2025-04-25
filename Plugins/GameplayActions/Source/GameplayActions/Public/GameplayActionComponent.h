#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayActionInterface.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"

#include "GameplayActionComponent.generated.h"

class UGameplayActionData;
class UGameplayAction;
class AActor;


UCLASS(ClassGroup = GameplayAction, hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEPLAYACTIONS_API UGameplayActionComponent : public UActorComponent, public IGameplayActionInterface
{
	GENERATED_BODY()

public:

	UGameplayActionComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//~ Begin IGameplayActionInterface Interface

	virtual bool CanStartAction(const FGameplayTag& InActionTag, TArray<FGameplayTag>& OutTagsToAbort) const override;

	virtual bool TryStartAction(const FGameplayTag& InActionTag) override;

	virtual void SetLoopingAction(const FGameplayTag& InActionTag, bool bTryStartImmediately) override;

	virtual void StopAction(const FGameplayTag& InActionTag, bool bActionCompleted) override;

	virtual void TriggerActionEvent(const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger) override;

	virtual bool IsActionActive(const FGameplayTag& InActionTag) const override;

	virtual TArray<FGameplayTag> GetActiveActions() const override;

	virtual void ClearLoopingAction(const FGameplayTag& InActionTag) override;

	virtual void AddAction(const FGameplayTag& InActionTag) override;

	virtual void RemoveAction(const FGameplayTag& InActionTag) override;

	virtual bool OwnsAction(const FGameplayTag& InActionTag) const override;

	virtual TArray<FGameplayTag> GetOwnedActions() const override;

	virtual void EnableAction(const FGameplayTag& InActionTag) override;

	virtual void DisableAction(const FGameplayTag& InActionTag) override;

	virtual bool IsActionEnabled(const FGameplayTag& InActionTag) const override;

	// action started delegates
	virtual FDelegateHandle AddActionStartedDelegate(const FGameplayActionStartedDelegate::FDelegate& InGameplayActionStartedDelegate) override;
	virtual bool RemoveActionStartedDelegate(FDelegateHandle DelegateHandle) override;
	virtual void AddActionStartedDynamicDelegate(FGameplayActionStartedDynamicDelegate InGameplayActionStartedDynamicDelegate) override;
	virtual void RemoveActionStartedDynamicDelegate(FGameplayActionStartedDynamicDelegate InGameplayActionStartedDynamicDelegate) override;

	// action stopped delegates
	virtual FDelegateHandle AddActionStoppedDelegate(const FGameplayActionStoppedDelegate::FDelegate& InGameplayActionStoppedDelegate) override;
	virtual bool RemoveActionStoppedDelegate(FDelegateHandle DelegateHandle) override;
	virtual void AddActionStoppedDynamicDelegate(FGameplayActionStoppedDynamicDelegate InGameplayActionStoppedDynamicDelegate) override;
	virtual void RemoveActionStoppedDynamicDelegate(FGameplayActionStoppedDynamicDelegate InGameplayActionStoppedDynamicDelegate) override;

	// action event triggered delegates
	virtual FDelegateHandle AddActionEventDelegate(const FGameplayActionEventDelegate::FDelegate& InGameplayActionEventDelegate) override;
	virtual bool RemoveActionEventDelegate(FDelegateHandle DelegateHandle) override;
	virtual void AddActionEventDynamicDelegate(FGameplayActionEventDynamicDelegate InGameplayActionEventDynamicDelegate) override;
	virtual void RemoveActionEventDynamicDelegate(FGameplayActionEventDynamicDelegate InGameplayActionEventDynamicDelegate) override;

	virtual void FireActionStartedDelegates(const FGameplayTag& InActionTag) override;
	virtual void FireActionStoppedDelegates(const FGameplayTag& InActionTag, bool bInActionAborted) override;

//~ End IGameplayActionInterface Interface

protected:

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	void ClearAllLoopingActions();

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	void StopAllActions();

	void FireActionEventDelegates(const FGameplayTag& InActionTag, const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger);

	UGameplayAction* GetOwnedAction(const FGameplayTag& InActionTag) const;

	const UGameplayActionData* GetOwnedActionData(const FGameplayTag& InActionTag) const;

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	TArray<const UGameplayActionData*> DefaultActionsData;

	UPROPERTY()
	TMap<FGameplayTag, const UGameplayActionData*> OwnedActionsData;

	UPROPERTY()
	TMap<FGameplayTag, UGameplayAction*> OwnedActions;

	UPROPERTY()
	TMap<FGameplayTag, FTimerHandle> LoopingActionsTimerHandles;

private:

	void AddDefaultActions();

	void OnStartLoopingAction(FGameplayTag InActionTag);

	UPROPERTY()
	TArray<FGameplayTag> ActiveActionsCache;

	UPROPERTY()
	TArray<FGameplayTag> DisabledActionsCache;

	UPROPERTY()
	TMap<FGameplayTag, FGameplayTag> ObservedEventToActionMapCache;

	FGameplayActionStartedDelegate GameplayActionStartedDelegates;
	FGameplayActionStoppedDelegate GameplayActionStoppedDelegates;
	FGameplayActionEventDelegate GameplayActionEventDelegates;

	UPROPERTY()
	TArray<FGameplayActionStartedDynamicDelegate> GameplayActionStartedDynamicDelegates;
	
	UPROPERTY()
	TArray<FGameplayActionStoppedDynamicDelegate> GameplayActionStoppedDynamicDelegates;

	UPROPERTY()
	TArray<FGameplayActionEventDynamicDelegate> GameplayActionEventDynamicDelegates;

};
