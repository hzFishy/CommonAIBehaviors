// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "AITypes.h"
#include "CAIBTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "CAIBPatrolTypes.generated.h"


	/*----------------------------------------------------------------------------
		Patrolling
	----------------------------------------------------------------------------*/
USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBPatrolSplinePointDataWaitAnimation
{
	GENERATED_BODY()
	
	FCAIBPatrolSplinePointDataWaitAnimation();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere)
	float PlayRate;
};

/** Data per spline point */
USTRUCT(DisplayName="Patrol Spline Point Data")
struct COMMONAIBEHAVIORS_API FCAIBPatrolSplinePointData
{
	GENERATED_BODY()

	FCAIBPatrolSplinePointData();

	/** Speed override to use while we walk to the target point */
	UPROPERTY(EditAnywhere, Category="Pre Point")
	TOptional<float> MaxWalkSpeed;

	/** Optional time to wait at this point once reached */
	UPROPERTY(EditAnywhere, Category="At Point")
	TOptional<float> WaitTime;
	
	/**
	 * Optional anim to play at this point, by default this skips WaitTime, we use the play duration instead.
	 * (see bOverrideAnimationDurationWithWaitTime to change that)
	 * If there is multiple animations we pick a random one.
	 */
	UPROPERTY(EditAnywhere, Category="At Point")
	TArray<FCAIBPatrolSplinePointDataWaitAnimation> WaitAnimationsData;

	/** If true we will use WaitTime over the duration of the anim picked with WaitAnimationsData */
	UPROPERTY(EditAnywhere, Category="At Point")
	bool bOverrideAnimationDurationWithWaitTime;
	
	// TODO: look at target while waiting
	// TODO: if multiple AI follow the same spline, adjust hteir pos to be next ot each other
};


/** Base struct for patrol runtime data */
USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBPatrolBaseRuntimeData : public FCAIBBehaviorRuntimeDataBase
{
	GENERATED_BODY()

public:
	FCAIBPatrolBaseRuntimeData();
	
	const FCAIBAIBehaviorPatrolFragment* CachedPatrolFragment;
	FAIRequestID CurrentMoveId;
	FVector CurrentTargetLocation;

	virtual void OnTargetActorSet() override;
	
	virtual void Start() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Stop() override;

#if CAIB_WITH_DEBUG
	virtual FFUMessageBuilder GetDebugState() const override;

	virtual void DrawDebugState() override;
#endif
	
protected:
	TWeakObjectPtr<AAIController> Controller;

	FDelegateHandle PathFinishDelegateHandle;
	bool bTargetPointReached;
	bool bMoving;

	virtual bool CanGoNextPoint() const;
	
	void MoveToNextPoint();

	virtual FVector GetNextMoveToLocation();

	virtual void OnPreStartMove();
	
	virtual void OnStartedMove();

	virtual void OnTargetPointReached();
};

USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBPatrolSplineRuntimeData : public FCAIBPatrolBaseRuntimeData
{
	GENERATED_BODY()

public:
	FCAIBPatrolSplineRuntimeData();

	TWeakObjectPtr<UAnimInstance> TargetCharacterAnimInstance;
	int32 InitialPointIndex;
	bool bTeleportToInitialPoint;
	TArray<FCAIBPatrolSplinePointData> SplinePointsData;
	
	void SetSplineActor(ACAIBPatrolSplineActor* InSplineActor);

	virtual void OnTargetActorSet() override;
	
	virtual void Start() override;

	virtual void Tick(float DeltaTime) override;

#if CAIB_WITH_DEBUG
	virtual FFUMessageBuilder GetDebugState() const override;
	
	virtual void DrawDebugState() override;
#endif
	
protected:
	TWeakObjectPtr<ACAIBPatrolSplineActor> SplineActor;
	int32 CachedSplineMaxIndex;
	
	float WaitElapsedTime;
	float WaitTargetTime;
	TOptional<float> PreviousMaxWalkSpeed;
	int32 CurrentTargetPointIndex;
	const FCAIBPatrolSplinePointDataWaitAnimation* SelectedWaitAnim;
	
	int32 PreviousTargetPointIndex;

	virtual bool CanGoNextPoint() const override;
	
	virtual FVector GetNextMoveToLocation() override;

	virtual void OnPreStartMove() override;
	
	virtual void OnStartedMove() override;
	
	virtual void OnTargetPointReached() override;
};


USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorPatrolBaseData
{
	GENERATED_BODY()
	FCAIBAIBehaviorPatrolBaseData();
	virtual ~FCAIBAIBehaviorPatrolBaseData();
	
	virtual TSharedPtr<FCAIBPatrolBaseRuntimeData> MakeSharedRuntime() const;
};

USTRUCT(DisplayName="Common AI Behavior Patrol Spline Data")
struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorPatrolSplineData : public FCAIBAIBehaviorPatrolBaseData
{
	GENERATED_BODY()
	
	FCAIBAIBehaviorPatrolSplineData();

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	FString StatusMessage;

	void PostSerialize(const FArchive& Ar);
	
	void ResfreshStatus();
#endif
	
	/** Spline uses as point source */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ACAIBPatrolSplineActor> PatrolSplineActor;

	UPROPERTY(EditAnywhere)
	int32 InitialPointIndex;
	
	UPROPERTY(EditAnywhere)
	bool bTeleportToInitialPoint;
	
	UPROPERTY(EditAnywhere)
	TArray<FCAIBPatrolSplinePointData> SplinePointsData;

	virtual TSharedPtr<FCAIBPatrolBaseRuntimeData> MakeSharedRuntime() const override;
};
#if WITH_EDITORONLY_DATA
template<>
struct TStructOpsTypeTraits<FCAIBAIBehaviorPatrolSplineData> : public TStructOpsTypeTraitsBase2<FCAIBAIBehaviorPatrolSplineData>
{
	enum
	{
		WithPostSerialize = true,
   };
};
#endif


USTRUCT(DisplayName="Common AI Behavior Patrol Fragment")
struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorPatrolFragment : public FCAIBAIBehaviorBaseFragment
{
	GENERATED_BODY()

	FCAIBAIBehaviorPatrolFragment();

	UPROPERTY(EditAnywhere)
	float AcceptanceRadius;
	
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TInstancedStruct<FCAIBAIBehaviorPatrolBaseData> Data;
};
