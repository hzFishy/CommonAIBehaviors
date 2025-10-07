// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "AITypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Core/CAIBCore.h"
#include "Utility/FUUtilities.h"
#include "CAIBTypes.generated.h"
class AAIController;
class ACAIBPatrolSplineActor;
class UCAIBAIBehaviorComponent;
using FU::Utils::FFUMessageBuilder;

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
USTRUCT(BlueprintType, DisplayName="Common AI Behavior Id")
struct FCAIBBehaviorId
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

USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBBehaviorRuntimeDataBase
{
	GENERATED_BODY()

public:
	FCAIBBehaviorRuntimeDataBase();
	virtual ~FCAIBBehaviorRuntimeDataBase();

	/** Called on first tick when this insatnce is added to the active array */
	virtual void Start();

	/** Called on next tick after Start was called */
	virtual void Tick(float DeltaTime);

	virtual void Stop();

#if CAIB_WITH_DEBUG
	virtual FFUMessageBuilder GetDebugState() const;

	virtual void DrawDebugState();
#endif
	
	bool HasStarted() const { return bStarted; };

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

	