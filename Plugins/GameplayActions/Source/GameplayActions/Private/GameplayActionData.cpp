#include "GameplayActionData.h"

#include "GameplayActionStartCondition.h"


FGameplayTag UGameplayActionData::GetActionTag() const
{
	return ActionTag;
}

TSubclassOf<UGameplayAction> UGameplayActionData::GetActionClass() const
{
	return ActionClass;
}

const UGameplayActionParameters* UGameplayActionData::GetActionParameters() const 
{
	return ActionParameters;
}

bool UGameplayActionData::CheckActionStartConditions(const UGameplayAction* InGameplayAction) const
{
	if (ensure(InGameplayAction != nullptr))
	{
		for (const UGameplayActionStartCondition* ActionStartCondition : ActionStartConditions)
		{
			if (ensure(ActionStartCondition != nullptr))
			{
				if (!ActionStartCondition->CheckCondition(InGameplayAction))
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

bool UGameplayActionData::CanAbortAllActions() const
{
	return bAbortAllActions;
}

const FGameplayTagContainer& UGameplayActionData::GetAbortActionTags() const
{
	return AbortActionTags;
}

const FGameplayTagContainer& UGameplayActionData::GetObservedEventTags() const
{
	return ObservedEventTags;
}
