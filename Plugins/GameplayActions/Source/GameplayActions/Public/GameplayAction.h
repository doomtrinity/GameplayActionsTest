#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "UObject/WeakInterfacePtr.h"

#include "GameplayAction.generated.h"

class IGameplayActionInterface;
class AActor;
class UGameplayActionParameters;


UCLASS(Abstract, BlueprintType, Blueprintable, meta = (ShowWorldContextPin))
class GAMEPLAYACTIONS_API UGameplayAction : public UObject
{
	GENERATED_BODY()

public:

	bool ShouldTickAction() const;

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayAction")
	void TickAction(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayAction")
	void OnActionEvent(const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger);

	UFUNCTION(BlueprintCallable, Category = "GameplayAction")
	bool IsActionActive() const;

	void InitAction(IGameplayActionInterface* InGameplayActionInterface, AActor* InOwnerActor, const FGameplayTag& InActionTag, const UGameplayActionParameters* InActionParameters);

	void StartAction();
	void StopAction(bool bInActionCompleted);

	bool IsActionEnabled() const;
	void SetActionEnabled(bool bInActionEnabled);
	
	AActor* GetOwnerActor() const;

	const UGameplayActionParameters* GetActionParameters() const;

	const FGameplayTag& GetActionTag() const;

	float GetLastStartTime() const;

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayAction")
	void OnInitAction();

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayAction")
	void OnActionStarted();

	UFUNCTION(BlueprintNativeEvent, Category = "GameplayAction")
	void OnActionStopped(bool bInActionAborted);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameplayAction")
	bool bTickActionFlagEnabled = false;

private:

	TWeakInterfacePtr<IGameplayActionInterface> GameplayActionInterface;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayAction", meta = (AllowPrivateAccess = "true"))
	AActor* OwnerActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayAction", meta = (AllowPrivateAccess = "true"))
	FGameplayTag ActionTag;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayAction", meta = (AllowPrivateAccess = "true"))
	const UGameplayActionParameters* ActionParameters = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayAction", meta = (AllowPrivateAccess = "true"))
	float LastStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayAction")
	bool bTickActionOnlyIfActive = true;
	
	UPROPERTY()
	bool bActionActive = false;

	UPROPERTY()
	bool bActionEnabled = true;

};
