// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Core/CAIBCore.h"
#include "Utility/FUUtilities.h"
#include "CAIBTypes.generated.h"
class UStateTree;
class AAIController;
class ACAIBPatrolSplineActor;
class UCAIBAIBehaviorComponent;
using FU::Utils::FFUMessageBuilder;

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
USTRUCT(BlueprintType, DisplayName="Common AI Behavior Id")
struct COMMONAIBEHAVIORS_API FCAIBBehaviorId
{
	GENERATED_BODY()

	FCAIBBehaviorId();
	FCAIBBehaviorId(FCAIBBehaviorId&& Other);
	FCAIBBehaviorId(const FCAIBBehaviorId& Other);
	FCAIBBehaviorId(uint32 InId);
	
	FCAIBBehaviorId& operator =(const FCAIBBehaviorId& Other)
	{
		Id = Other.Id;
		return *this;
	};
	
	bool operator==(const FCAIBBehaviorId& Other) const
	{
		return Id == Other.Id;
	};

	friend uint32 GetTypeHash(const FCAIBBehaviorId& Self)
	{
		return GetTypeHash(Self.Id);
	}
	
	bool IsValid() const
	{
		return Id != 0;
	};
	
protected:
	uint32 Id;
};


struct COMMONAIBEHAVIORS_API FCAIBStateTreeCacheId
{
	FCAIBStateTreeCacheId();
	FCAIBStateTreeCacheId(const UStateTree* InStateTree, const UScriptStruct* InTaskType);

	bool operator==(const FCAIBStateTreeCacheId& Other) const
	{
		return StateTree == Other.StateTree && TaskType == Other.TaskType;
	}
	
	friend uint32 GetTypeHash(const FCAIBStateTreeCacheId& Self)
	{
		return HashCombineFast(Self.StateTree.GetWeakPtrTypeHash(), Self.TaskType.GetWeakPtrTypeHash());
	}
	
protected:
	TWeakObjectPtr<const UStateTree> StateTree;
	TWeakObjectPtr<const UScriptStruct> TaskType;
};


USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBBehaviorRuntimeDataBase
{
	GENERATED_BODY()

public:
	FCAIBBehaviorRuntimeDataBase();
	virtual ~FCAIBBehaviorRuntimeDataBase();

	/** Called on first tick when this insatnce is added to the active array */
	virtual void Start();

	virtual bool CanTick();
	
	/** Called on next tick after Start was called */
	virtual void Tick(float DeltaTime);

	virtual void Stop();
	
	virtual void Pause();
	
	virtual void Resume();

#if CAIB_WITH_DEBUG
	/**
	 * Used to return custom runtime data.
	 * Called each tick.
	 */
	virtual FFUMessageBuilder GetDebugState() const;

	/**
	 *	Used to draw extra stuff.
	 *  Called each tick.
	 */
	virtual void DrawDebugState();
#endif
	
	bool HasStarted() const { return bStarted; };

	bool IsActive() const { return bActive; };

	void SetTargetActor(AActor* InActor);

	virtual void OnTargetActorSet();
	
	AActor* GetTargetActor() const { return TargetActor.Get(); };
	
	template<std::derived_from<AActor> T>
	T* GetTargetActor() const
	{
		return Cast<T>(TargetActor.Get());
	};

	ACharacter* GetTargetCharacter() const;

	template<std::derived_from<UActorComponent> T>
	T* GetTargetComponent() const
	{
		return GetTargetActor()->GetComponentByClass<T>();
	}
	
protected:
	bool bStarted;
	/** The actor that is "executing" the behavior, this should be the AI Pawn or Character */
	TWeakObjectPtr<AActor> TargetActor;
	/** if active we will call tick */
	bool bActive;
};


USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorBaseFragment
{
	GENERATED_BODY()
	
	FCAIBAIBehaviorBaseFragment();
};


struct COMMONAIBEHAVIORS_API FCAIBAIBehaviorDebugMessageEntry
{
	FCAIBAIBehaviorDebugMessageEntry();

	TWeakObjectPtr<AActor> TargetActor;
	
	FFUMessageBuilder MessageBuilder;
};

	