#include "StartConditions/CooldownStartCondition.h"

#include "GameplayAction.h"
#include "GameplayActionSubsystem.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"


bool UCooldownStartCondition::IsConditionSatisfied_Implementation(const UGameplayAction* InGameplayAction) const
{
	if (InGameplayAction != nullptr && InGameplayAction->GetOwnerActor() != nullptr)
	{
		const AActor* OwnerActor = InGameplayAction->GetOwnerActor();
		const float CurrentTime = OwnerActor->GetWorld() != nullptr ? OwnerActor->GetWorld()->GetTimeSeconds() : 0.0f;
		const float LastOwnerStartTime = InGameplayAction->GetLastStartTime();
		float NextActionTime = LastOwnerStartTime + DefaultCooldownTime;
		
		if (CurrentTime < NextActionTime)
		{
			// no need to check global time if owner time check fails
			return false;
		}
		
		if (bIgnoreGlobalActionStartTime)
		{
			return true;
		}

		if (UGameplayActionSubsystem* GameplayActionSubsystem = UGameplayActionSubsystem::Get(OwnerActor))
		{
			const float LastGlobalStartTime = GameplayActionSubsystem->GetLastGameplayActionStartTime(InGameplayAction->GetActionTag());
			NextActionTime = LastGlobalStartTime + DefaultGlobalCooldownTime;
			return CurrentTime > NextActionTime;
		}
	}
	return false;
}
