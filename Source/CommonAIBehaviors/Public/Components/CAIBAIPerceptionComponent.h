// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "GameplayTagContainer.h"
#include "Data/CAIBPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "CAIBAIPerceptionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCAIBOnNewSensePerceivedForTargetSignature,
	UCAIBAIPerceptionComponent*, PerceptionComponent,
	FGameplayTag, SenseTag,
	const FCAIBTrackedStimulusSource&, TrackedSource
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCAIBOnUpdatedSensePerceivedForTargetSignature,
	UCAIBAIPerceptionComponent*, PerceptionComponent,
	FGameplayTag, SenseTag,
	const FCAIBTrackedStimulusSource&, TrackedSource
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCAIBOnForgottenSensePerceivedForTargetSignature,
	UCAIBAIPerceptionComponent*, PerceptionComponent,
	FGameplayTag, SenseTag
);


struct COMMONAIBEHAVIORS_API FCAIBStimuliExpired
{
	FCAIBStimuliExpired();
	FCAIBStimuliExpired(int32 InId, FGameplayTag InTag);
	
	int32 SourceId;
	FGameplayTag SenseTag;
};


UCLASS(ClassGroup=(CommonAIBehaviors), DisplayName="Common AI Perception Component", meta=(BlueprintSpawnableComponent))
class COMMONAIBEHAVIORS_API UCAIBAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
public:
	UPROPERTY(BlueprintAssignable, DisplayName="On New Sense Perceived For Target")
	FCAIBOnNewSensePerceivedForTargetSignature OnNewSensePerceivedForTargetDelegate;
	
	UPROPERTY(BlueprintAssignable, DisplayName="On Updated Sense Perceived For Target")
	FCAIBOnUpdatedSensePerceivedForTargetSignature OnUpdatedSensePerceivedForTargetDelegate;
	
	/** This will be called before OnTargetPerceptionForgotten */
	UPROPERTY(BlueprintAssignable, DisplayName="On Forgotten Sense Perceived For Target")
	FCAIBOnForgottenSensePerceivedForTargetSignature OnForgottenSensePerceivedForTargetDelegate;
	
protected:
	/** How much we wait between ticks, if not set we tick on each frame */
	UPROPERTY(EditAnywhere, Category="Common AI Behavior")
	TOptional<float> TickInterval;

	float ElapsedTimeSinceLastTick;
	
	/** Key: SourceId, value holds a list of all various sense that "found" the same source */
	TMap<int32, FCAIBTrackedSensesContainer> TrackedSources;

	TArray<FCAIBStimuliExpired> ExpiredSources;

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAIPerceptionComponent();

	virtual void OnRegister() override;

	virtual void OnUnregister() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
protected:
	virtual void OnNewPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	virtual void OnInvalidPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	virtual void OnPerceptionSourceForgotten(int32 SourceId, FGameplayTag SenseTag);

	void TickTrackedSources(float DeltaTime);

	virtual void RemoveStimulus(const FCAIBStimuliExpired& ExpiredSource);
	
	
	/*----------------------------------------------------------------------------
		Callback
	----------------------------------------------------------------------------*/
protected:
	UFUNCTION() virtual void TargetPerceptionInfoUpdatedCallback(const FActorPerceptionUpdateInfo& UpdateInfo);
};
