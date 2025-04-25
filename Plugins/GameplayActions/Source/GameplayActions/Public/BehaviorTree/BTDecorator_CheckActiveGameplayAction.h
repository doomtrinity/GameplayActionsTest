#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"

#include "BTDecorator_CheckActiveGameplayAction.generated.h"

class AActor;
class IGameplayActionInterface;


struct FBTCheckActiveGameplayActionMemory
{
	FDelegateHandle ActionStartedDelegateHandle;
};


UCLASS()
class GAMEPLAYACTIONS_API UBTDecorator_CheckActiveGameplayAction : public UBTDecorator
{
	GENERATED_BODY()

public:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;

protected:

	UBTDecorator_CheckActiveGameplayAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTCheckActiveGameplayActionMemory); }

	UPROPERTY(EditAnywhere, Category = "GameplayAction")
	FGameplayTagContainer ActiveActionTags;

private:

	void OnActionStarted(UBehaviorTreeComponent& OwnerComp, AActor* InActionOwner, const FGameplayTag& InActionTag) const;
	bool HasActiveActions(const IGameplayActionInterface* InGameplayActionInterface) const;
};
