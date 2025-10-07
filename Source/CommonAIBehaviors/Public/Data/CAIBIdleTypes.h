// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CAIBTypes.h"
#include "CAIBIdleTypes.generated.h"

	
	/*----------------------------------------------------------------------------
		Static Idle
	----------------------------------------------------------------------------*/
USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBehaviorBaseData
{
	GENERATED_BODY()

	FCAIBStaticIdleBehaviorBaseData();
};


/**
 * Single animation that loops.
 */
USTRUCT(DisplayName="Common AI Behavior Static Idle Single Data")
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBehaviorSingleData : public FCAIBStaticIdleBehaviorBaseData
{
	GENERATED_BODY()

	FCAIBStaticIdleBehaviorSingleData();

	/**
	 * Animation the AI will play.
	 * Should be seamless and set to autoloop since we are only going to play this once
	 */
	UPROPERTY(EditAnywhere, DisplayName="Montage")
	TSoftObjectPtr<UAnimMontage> SoftMontage;
};


/** Defines how we should pick the next index in the sequence array */
UENUM(DisplayName="Common AI Behavior Static Idle Sequence Type")
enum class ECAIBStaticIdleBehaviorSequenceType
{
	/** Play the sequence entries in the array order */
	InOrder,
	/** Pick a random sequence entry, exclude the previously selected entry */
	RandomExcludeSelf,
	/** Pick a random sequence entry, this can select the previously selected entry */
	RandomAll
};

USTRUCT(DisplayName="Common AI Behavior Static Idle Sequence Entry Data")
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBehaviorSequenceEntryData
{
	GENERATED_BODY()
	
	FCAIBStaticIdleBehaviorSequenceEntryData();
	
	/**
	 * Animation the AI will play.
	 * Can be seamless and set to autoloop since we are only going to play this once
	 * but you might want to play this sequence entry longer that the animation duration.
	 * This can be interupted if Duration is set.
	 */
	UPROPERTY(EditAnywhere, DisplayName="Montage")
	TSoftObjectPtr<UAnimMontage> SoftMontage;
	
	UPROPERTY(EditAnywhere)
	float MontagePlayRate;

	/** If set this entry will be active for Duration instead of using the play duration of the Montage */
	UPROPERTY(EditAnywhere)
	TOptional<float> Duration;
};

/**
 * Sequence of animations and more.
 */
USTRUCT(DisplayName="Common AI Behavior Static Idle Sequence Data")
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBehaviorSequenceData : public FCAIBStaticIdleBehaviorBaseData
{
	GENERATED_BODY()

	FCAIBStaticIdleBehaviorSequenceData();

	UPROPERTY(EditAnywhere)
	ECAIBStaticIdleBehaviorSequenceType SelectionType;
	
	UPROPERTY(EditAnywhere)
	TArray<FCAIBStaticIdleBehaviorSequenceEntryData> SequenceEntries;
};


USTRUCT(DisplayName="Common AI Behavior Static Idle Fragment")
struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorStaticIdleFragment : public FCAIBAIBehaviorBaseFragment
{
	GENERATED_BODY()

	FCAIBAIBehaviorStaticIdleFragment();

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TInstancedStruct<FCAIBStaticIdleBehaviorBaseData> Data;
};

USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleRuntimeData : public FCAIBBehaviorRuntimeDataBase
{
	GENERATED_BODY()

public:
	FCAIBStaticIdleRuntimeData();
	
	const FCAIBStaticIdleBehaviorSequenceData* SequenceData;

	virtual void OnTargetActorSet() override;
	
	virtual void Start() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Stop() override;

#if CAIB_WITH_DEBUG
	virtual FFUMessageBuilder GetDebugState() const override;
#endif
	
protected:
	TWeakObjectPtr<UAnimInstance> CharacterAnimInstance;
	
	/** Index used in the latest sequence selection */
	int32 CurrentIndex;

	/** Anim Montage the character is currently playing */
	TWeakObjectPtr<UAnimMontage> CurrentMontage;
	
	/** How much we wait until we trigger a new selection */
	float TargetRelativeTime;
	
	/** Elapsed game time since we selected a new sequence entry */
	float ElapsedTime;
	
	int32 GetNextSequenceEntryIndex();

	void GoNextSequenceEntry();
};
