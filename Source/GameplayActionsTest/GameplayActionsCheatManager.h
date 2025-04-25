#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"

#include "GameplayActionsCheatManager.generated.h"

class AActor;
class UActorComponent;
class IGameplayActionInterface;
struct FGameplayTag;


UCLASS()
class GAMEPLAYACTIONSTEST_API UGameplayActionsCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	
	UFUNCTION(exec, Category = "GameplayAction")
	void GameplayActions_AddActionStartedDelegate();

	UFUNCTION(exec, Category = "GameplayAction")
	void GameplayActions_RemoveActionStartedDelegate();

	UFUNCTION(exec, BlueprintImplementableEvent, Category = "GameplayAction")
	void GameplayActions_AddActionStartedDynamicDelegate();

	UFUNCTION(exec, BlueprintImplementableEvent, Category = "GameplayAction")
	void GameplayActions_RemoveActionStartedDynamicDelegate();

	UFUNCTION(exec, Category = "GameplayAction")
	void GameplayActions_AddActionStoppedDelegate();

	UFUNCTION(exec, Category = "GameplayAction")
	void GameplayActions_RemoveActionStoppedDelegate();

	UFUNCTION(exec, BlueprintImplementableEvent, Category = "GameplayAction")
	void GameplayActions_AddActionStoppedDynamicDelegate();

	UFUNCTION(exec, BlueprintImplementableEvent, Category = "GameplayAction")
	void GameplayActions_RemoveActionStoppedDynamicDelegate();

	UFUNCTION(exec, Category = "GameplayAction")
	void GameplayActions_AddActionEventDelegate();

	UFUNCTION(exec, Category = "GameplayAction")
	void GameplayActions_RemoveActionEventDelegate();

	UFUNCTION(exec, BlueprintImplementableEvent, Category = "GameplayAction")
	void GameplayActions_AddActionEventDynamicDelegate();

	UFUNCTION(exec, BlueprintImplementableEvent, Category = "GameplayAction")
	void GameplayActions_RemoveActionEventDynamicDelegate();

protected:

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	UActorComponent* FindFirstGameplayActionComponent() const;

private:

	void OnActionStarted(AActor* InActionOwner, const FGameplayTag& InActionTag);
	void OnActionStopped(AActor* InActionOwner, const FGameplayTag& InActionTag, bool bInActionAborted);
	void OnActionEvent(AActor* InActionOwner, const FGameplayTag& InActionTag, const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger);

	IGameplayActionInterface* FindFirstGameplayActionInterface() const;
	
	FDelegateHandle ActionStartedDelegateHandle;
	FDelegateHandle ActionStoppedDelegateHandle;
	FDelegateHandle ActionEventDelegateHandle;
};
