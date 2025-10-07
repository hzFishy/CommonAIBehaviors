// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Data/CAIBTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "CAIBBehaviorSubsystem.generated.h"


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
	TMap<FCAIBBehaviorId, TSharedPtr<FCAIBBehaviorRuntimeDataBase>> ActiveBehaviors;

	uint32 LatestBehaviorId;
	uint32 LatestPermanentDebugId;

	TMap<uint32, FCAIBAIBehaviorDebugMessageEntry> PermanentDebugMessages;

#if CAIB_WITH_DEBUG
	TWeakObjectPtr<AActor> PlayerActor;
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
	FCAIBBehaviorId MakeUniqueId();

	FCAIBBehaviorId AddBehavior(TSharedPtr<FCAIBBehaviorRuntimeDataBase> Val);

	void StopAndRemoveBehavior(const FCAIBBehaviorId& BehaviorId);

	uint32 AddDebugMessage(const FCAIBAIBehaviorDebugMessageEntry& Entry);

	void RemoveDebugMessage(uint32 Id);
	
protected:
	void TickActiveBehaviors(float DeltaTime);

#if CAIB_WITH_DEBUG
	void TickDebugMessages(float DeltaTime);
#endif
};
