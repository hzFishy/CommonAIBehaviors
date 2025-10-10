// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CAIBTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "CAIBIdleTypes.generated.h"
class UPathFollowingComponent;

	
	/*----------------------------------------------------------------------------
		Static Idle
	----------------------------------------------------------------------------*/
USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBaseRuntimeData : public FCAIBBehaviorRuntimeDataBase
{
	GENERATED_BODY()

public:
	FCAIBStaticIdleBaseRuntimeData();
	
	virtual void OnTargetActorSet() override;
	
	virtual void Pause() override;

	virtual void Resume() override;
	
	virtual void Stop() override;

	const FCAIBStaticIdleBehaviorBaseData* CachedBaseData;

	template <std::derived_from<FCAIBStaticIdleBehaviorBaseData> T>
	const T* GetCachedDataAs() const
	{
		return static_cast<const T*>(CachedBaseData);
	}
	
protected:
	TWeakObjectPtr<UAnimInstance> CharacterAnimInstance;
	TWeakObjectPtr<AAIController> Controller;
	TWeakObjectPtr<UPathFollowingComponent> PathFollowingComponent;
	FDelegateHandle PathFinishDelegateHandle;
	FVector StartLocation;
	FRotator StartRotation;
	bool bMovingToStart;
	
	/** Anim Montage the character is currently playing */
	TWeakObjectPtr<UAnimMontage> CurrentMontage;

	virtual void OnResumeFinished();
	
	void StopCurrentAnimation();
};

USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleSingleRuntimeData : public FCAIBStaticIdleBaseRuntimeData
{
	GENERATED_BODY()

public:
	FCAIBStaticIdleSingleRuntimeData();
	
	virtual void Start() override;

	virtual void Pause() override;

#if CAIB_WITH_DEBUG
	virtual FFUMessageBuilder GetDebugState() const override;
#endif

protected:
	const FCAIBStaticIdleBehaviorSingleData* CachedSingleData;

	virtual void OnResumeFinished() override;
	
	void PlaySingleAnim();
};

USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleSequenceRuntimeData : public FCAIBStaticIdleBaseRuntimeData
{
	GENERATED_BODY()

public:
	FCAIBStaticIdleSequenceRuntimeData();
	
	virtual void Start() override;

	virtual bool CanTick() override;
	
	virtual void Tick(float DeltaTime) override;

#if CAIB_WITH_DEBUG
	virtual FFUMessageBuilder GetDebugState() const override;
#endif
	
protected:
	const FCAIBStaticIdleBehaviorSequenceData* CachedSequenceData;
	
	/** Index used in the latest sequence selection */
	int32 CurrentIndex;

	/** How much we wait until we trigger a new selection */
	float TargetRelativeTime;
	
	/** Elapsed game time since we selected a new sequence entry */
	float ElapsedTime;

	virtual void OnResumeFinished() override;
	
	int32 GetNextSequenceEntryIndex();

	void GoNextSequenceEntry();
	
	void ExecuteCurrentSequenceEntry();
};


USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBehaviorBaseData
{
	GENERATED_BODY()
	
	FCAIBStaticIdleBehaviorBaseData();
	virtual ~FCAIBStaticIdleBehaviorBaseData();

	virtual TSharedPtr<FCAIBStaticIdleBaseRuntimeData> MakeSharedRuntime() const;

	
	/**
	 * On first play we store the AI location as the "StartLocation".
	 * The idle state might be aborted because of external events (e.i: detecting and chasing the player)
	 * This means that when the AI will go back to the idle state it might be at an undesired location.
	 * If this is true, we will only replay the montage after we successfully reach the start location
	 */
	UPROPERTY(EditAnywhere)
	bool bOnResumeGoBackToStartLocation;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bOnResumeGoBackToStartLocation"))
	bool bOnResumeGoBackToStartRotation;
	
	UPROPERTY(EditAnywhere, meta=(EditCondition="bOnResumeGoBackToStartLocation"))
	float GoBackToStartAcceptanceRadius;

	// TODO: random rotation on start between 2 yaw angles
};


/**
 * Single animation that loops.
 */
USTRUCT(DisplayName="Common AI Behavior Static Idle Single Data")
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleBehaviorSingleData : public FCAIBStaticIdleBehaviorBaseData
{
	GENERATED_BODY()

	FCAIBStaticIdleBehaviorSingleData();

	virtual TSharedPtr<FCAIBStaticIdleBaseRuntimeData> MakeSharedRuntime() const override;
	
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

	virtual TSharedPtr<FCAIBStaticIdleBaseRuntimeData> MakeSharedRuntime() const override;

	
	UPROPERTY(EditAnywhere)
	ECAIBStaticIdleBehaviorSequenceType SelectionType;
	
	UPROPERTY(EditAnywhere)
	TArray<FCAIBStaticIdleBehaviorSequenceEntryData> SequenceEntries;

	/** When we resume this behavior do we restart at 0? */
	UPROPERTY(EditAnywhere)
	bool bOnResumeGoBackToInitialIndex;

	UPROPERTY(EditAnywhere)
	bool bOnResumeResetAnimationProgress;
};


USTRUCT(DisplayName="Common AI Behavior Static Idle Fragment")
struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorStaticIdleFragment : public FCAIBAIBehaviorBaseFragment
{
	GENERATED_BODY()

	FCAIBAIBehaviorStaticIdleFragment();

	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TInstancedStruct<FCAIBStaticIdleBehaviorBaseData> Data;
};
