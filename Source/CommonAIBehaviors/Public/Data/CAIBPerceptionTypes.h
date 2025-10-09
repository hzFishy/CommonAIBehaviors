// By hzFishy - 2025 - Do whatever you want with it.

#pragma once


#include "Perception/AIPerceptionComponent.h"
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


struct COMMONAIBEHAVIORS_API CAIBTrackedStimuliSource
{
	CAIBTrackedStimuliSource();
	
	CAIBTrackedStimuliSource(const FActorPerceptionUpdateInfo& UpdateInfo);

	/** Id of to the stimulus source */
	int32 TargetId;

	/** Actor associated to the stimulus (can be null) */
	TWeakObjectPtr<AActor> Target;

	/** Updated stimulus */
	FAIStimulus LatestStimulus;
};
