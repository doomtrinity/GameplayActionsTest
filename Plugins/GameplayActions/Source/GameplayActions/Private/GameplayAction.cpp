#include "GameplayAction.h"

#include "GameFramework/Actor.h"
#include "GameplayActionInterface.h"
#include "GameplayActionParameters.h"
#include "GameplayActionSubsystem.h"
#include "Engine/World.h"


void UGameplayAction::InitAction(IGameplayActionInterface* InGameplayActionInterface, AActor* InOwnerActor, const FGameplayTag& InActionTag, const UGameplayActionParameters* InActionParameters)
{
	GameplayActionInterface = InGameplayActionInterface;
	OwnerActor = InOwnerActor;
	ActionTag = InActionTag;
	ActionParameters = InActionParameters;

	OnInitAction();
}

void UGameplayAction::StartAction()
{
	if (ensure(bActionEnabled) && ensure(!bActionActive))
	{
		bActionActive = true;

		// set last start time
		if (OwnerActor != nullptr) 
		{
			const float CurrentTime = OwnerActor->GetWorld() != nullptr ? OwnerActor->GetWorld()->GetTimeSeconds() : 0.0f;
			LastStartTime = CurrentTime;
			if (UGameplayActionSubsystem* GameplayActionSubsystem = UGameplayActionSubsystem::Get(OwnerActor))
			{
				GameplayActionSubsystem->SetLastGameplayActionStartTime(ActionTag, LastStartTime);
			}
		}

		OnActionStarted();

		// the event stopped the action (in the same frame), this is probably wrong
		ensure(bActionActive);
		
		// fire the action started delegates
		if (GameplayActionInterface.IsValid())
		{
			GameplayActionInterface->FireActionStartedDelegates(ActionTag);
		}
	}
}

void UGameplayAction::StopAction(bool bInActionCompleted)
{
	if (ensure(bActionEnabled) && ensure(bActionActive))
	{
		bActionActive = false;
		const bool bActionAborted = !bInActionCompleted;
		OnActionStopped(bActionAborted);
		
		// the event restarted the action (in the same frame), this is probably wrong
		ensure(!bActionActive);

		// fire the action stopped delegates
		if (GameplayActionInterface.IsValid())
		{
			GameplayActionInterface->FireActionStoppedDelegates(ActionTag, bActionAborted);
		}
	}
}

bool UGameplayAction::IsActionEnabled() const
{
	return bActionEnabled;
}

void UGameplayAction::SetActionEnabled(bool bInActionEnabled) 
{
	bActionEnabled = bInActionEnabled;
}

void UGameplayAction::OnInitAction_Implementation()
{
	// nothing to do for now
}

bool UGameplayAction::ShouldTickAction() const
{
	if (bActionEnabled && bTickActionFlagEnabled)
	{
		if (bTickActionOnlyIfActive && !bActionActive) 
		{
			return false;
		}
		return true;
	}
	return false;
}

void UGameplayAction::TickAction_Implementation(float DeltaTime)
{
	// nothing to do for now
}

void UGameplayAction::OnActionStarted_Implementation()
{
	// nothing to do for now
}

void UGameplayAction::OnActionStopped_Implementation(bool bInActionAborted)
{
	// nothing to do for now
}

void UGameplayAction::OnActionEvent_Implementation(const FGameplayTag& InEventTag, AActor* InInstigator, AActor* InTrigger)
{
	// nothing to do for now
}

bool UGameplayAction::IsActionActive() const
{
	return bActionActive;
}

AActor* UGameplayAction::GetOwnerActor() const
{
	return OwnerActor;
}

const UGameplayActionParameters* UGameplayAction::GetActionParameters() const 
{
	return ActionParameters;
}

const FGameplayTag& UGameplayAction::GetActionTag() const
{
	return ActionTag;
}

float UGameplayAction::GetLastStartTime() const
{
	return LastStartTime;
}
