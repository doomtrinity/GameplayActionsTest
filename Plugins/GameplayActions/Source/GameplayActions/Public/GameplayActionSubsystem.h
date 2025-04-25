#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/WeakInterfacePtr.h"
#include "GameplayTagContainer.h"

#include "GameplayActionSubsystem.generated.h"

class IGameplayActionInterface;
class UObject;
class AActor;


UCLASS()
class GAMEPLAYACTIONS_API UGameplayActionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	static UGameplayActionSubsystem* Get(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	void BroadcastGameplayActionEvent(const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	float GetLastGameplayActionStartTime(const FGameplayTag& InActionTag) const;

	void SetLastGameplayActionStartTime(const FGameplayTag& InActionTag, float StartTime);

	void RegisterGameplayActionInterface(IGameplayActionInterface* InGameplayActionInterface);
	void UnregisterGameplayActionInterface(IGameplayActionInterface* InGameplayActionInterface);

private:

	TArray<TWeakInterfacePtr<IGameplayActionInterface>> GameplayActionInterfaces;

	TMap<FGameplayTag, float> LastGameplayActionsStartTime;
	
};
