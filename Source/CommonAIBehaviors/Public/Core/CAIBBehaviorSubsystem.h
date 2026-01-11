// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Data/CAIBTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "CAIBBehaviorSubsystem.generated.h"
class UCAIBAIBehaviorObject;


/**
 * 
 */
UCLASS(DisplayName="Common AI Behavior Subsystem")
class COMMONAIBEHAVIORS_API UCAIBBehaviorSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
public:
	TMap<FCAIBBehaviorId, TSharedPtr<FCAIBBehaviorRuntimeDataBase>> RuntimeBehaviors;

	/**
	 * This works as long as there is only one use of task type per state tree.
	 */
	TMap<FCAIBStateTreeCacheId, FCAIBBehaviorId> CachedStateTreeBehaviorIds;
	
	uint32 LatestBehaviorId;
	uint32 LatestPermanentDebugId;

	TMap<uint32, FCAIBAIBehaviorDebugMessageEntry> PermanentDebugMessages;
	
	UPROPERTY()
	TArray<TObjectPtr<UCAIBAIBehaviorObject>> BehaviorObjects;
	
#if CAIB_WITH_DEBUG
	TWeakObjectPtr<AActor> DebugPlayerActor;

	FDelegateHandle OnActorAddedToWorldHandle;
	FDelegateHandle OnActorRemovedToWorldHandle;
	int32 DebugCurrentTargetIndex;
	TArray<TWeakObjectPtr<APawn>> DebugTargetPawns;
#endif
	
	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBBehaviorSubsystem();
	
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCAIBBehaviorSubsystem, STATGROUP_Tickables); }

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	/** Mainly used by AddActiveBehavior to generate a unique behavior id but can be used for "ghost" behaviors */
	FCAIBBehaviorId MakeUniqueId();

	FCAIBBehaviorId AddActiveBehavior(TSharedPtr<FCAIBBehaviorRuntimeDataBase> Val);

	bool HasBehavior(const FCAIBBehaviorId& BehaviorId) const;
	
	void PauseBehavior(const FCAIBBehaviorId& BehaviorId);
	
	void ResumeBehavior(const FCAIBBehaviorId& BehaviorId);
	
	void StopAndRemoveBehavior(const FCAIBBehaviorId& BehaviorId);

	void CacheNewBehaviorid(const FCAIBStateTreeCacheId& CacheId, const FCAIBBehaviorId& BehaviorId);
	
	const FCAIBBehaviorId& GetCachedBehaviorid(const FCAIBStateTreeCacheId& CacheId);
	
	FCAIBBehaviorId* GetCachedBehavioridSafe(const FCAIBStateTreeCacheId& CacheId);
	
	uint32 AddDebugMessage(const FCAIBAIBehaviorDebugMessageEntry& Entry);

	void RemoveDebugMessage(uint32 Id);

	UFUNCTION(BlueprintCallable, Category="Common AI Behaviors")
	void AddBehaviorObject(UCAIBAIBehaviorObject* Object);
	
protected:
	void TickBehaviors(float DeltaTime);

	
	/*----------------------------------------------------------------------------
		Debug
	----------------------------------------------------------------------------*/
#if CAIB_WITH_DEBUG
public:
	void GoToNextDebugAIPawn();
	
	void GoToPreviousDebugAIPawn();
	
protected:
	void TickDebugMessages(float DeltaTime);

	void OnActorAddedToWorld(AActor* Actor);
	
	void OnDebugAIPawnAddedToWorld(APawn* InPawn);
	
	void OnActorRemovedToWorld(AActor* Actor);
	
	void OnDebugAIPawnRemovedToWorld(APawn* InPawn);

	APawn* GetTargetDebugAIPawn();
#endif
};
