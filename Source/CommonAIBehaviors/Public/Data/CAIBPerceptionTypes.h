// By hzFishy - 2025 - Do whatever you want with it.

#pragma once


#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "CAIBPerceptionTypes.generated.h"


UCLASS(DisplayName="Common AI Sense Sight")
class COMMONAIBEHAVIORS_API UCAIBAISense_Sight : public UAISense_Sight
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAISense_Sight();
};

UCLASS(DisplayName="Common AI Sense Sight Config")
class COMMONAIBEHAVIORS_API UCAIBAISenseConfig_Sight : public UAISenseConfig_Sight
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAISenseConfig_Sight();

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
	
	// state tree event tag to trigger on first perception
	// state tree event tag to trigger on perception forgotten
};
