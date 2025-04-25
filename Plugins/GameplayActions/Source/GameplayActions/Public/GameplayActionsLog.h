#pragma once

#include "CoreMinimal.h"

GAMEPLAYACTIONS_API DECLARE_LOG_CATEGORY_EXTERN(LogGameplayAction, Log, All);


#define GAMEPLAY_ACTION_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogGameplayAction, Verbosity, TEXT("(%s:%d) ") Format, ANSI_TO_TCHAR(__FUNCTION__), __LINE__ , ##__VA_ARGS__) \
}

// TODO add visual logging? Requires overriding AAIController::GrabDebugSnapshot
