#include "GameplayActionParameters.h"

#include "GameFramework/Actor.h"


float UGameplayActionParameters::GetTimeBetweenLoops_Implementation(const AActor* TargetActor) const
{
	return TimeBetweenLoops;
}

