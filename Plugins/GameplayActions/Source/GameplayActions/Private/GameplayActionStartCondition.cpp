#include "GameplayActionStartCondition.h"

#include "GameplayAction.h"
#include "GameplayActionsLog.h"


bool UGameplayActionStartCondition::IsConditionSatisfied_Implementation(const UGameplayAction* InGameplayAction) const
{
	ensureMsgf(false, TEXT("IsConditionSatisfied not implemented in condition! [%s]"), *GetNameSafe(this));
	return false;
}

bool UGameplayActionStartCondition::CheckCondition(const UGameplayAction* InGameplayAction) const
{
	if (bIgnoreCheck) 
	{
		GAMEPLAY_ACTION_LOG(Warning, TEXT("ignored action start condition check: [%s]"), *GetNameSafe(this));
		return true;
	}
	const bool bCheckResult = IsConditionSatisfied(InGameplayAction);
	GAMEPLAY_ACTION_LOG(Verbose, TEXT("checked gameplay action start condition: [%s], result: [%d]"), *GetNameSafe(this), bCheckResult);
	return bCheckResult;

}

