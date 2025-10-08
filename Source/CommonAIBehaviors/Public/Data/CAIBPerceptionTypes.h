// By hzFishy - 2025 - Do whatever you want with it.

#pragma once


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

	// state tree event tag to trigger on first perception
	// state tree event tag to trigger on perception forgotten
};
