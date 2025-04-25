#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"

#include "BTService_StartGameplayActionByEvent.generated.h"

class AActor;


struct FBTStartGameplayActionByEventMemory
{
	FDelegateHandle ActionEventDelegateHandle;
};


UCLASS(hidecategories = (Service))
class GAMEPLAYACTIONS_API UBTService_StartGameplayActionByEvent : public UBTService
{
	GENERATED_BODY()

protected:

	UBTService_StartGameplayActionByEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTStartGameplayActionByEventMemory); }
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	FGameplayTagContainer ObservedActionEventTags;

private:

	void OnActionEvent(AActor* InActionOwner, const FGameplayTag& InActionTag, const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger);

};
