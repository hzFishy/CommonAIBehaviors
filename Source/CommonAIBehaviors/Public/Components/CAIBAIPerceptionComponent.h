// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Perception/AIPerceptionComponent.h"
#include "CAIBAIPerceptionComponent.generated.h"


UCLASS(ClassGroup=(CommonAIBehaviors), DisplayName="Common AI Perception Component", meta=(BlueprintSpawnableComponent))
class COMMONAIBEHAVIORS_API UCAIBAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAIPerceptionComponent();
};
