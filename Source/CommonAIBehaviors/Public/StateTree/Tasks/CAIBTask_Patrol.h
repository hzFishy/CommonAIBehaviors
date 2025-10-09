// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "StateTreeTaskBase.h"
#include "Data/CAIBTypes.h"
#include "CAIBTask_Patrol.generated.h"
class ACAIBPatrolSplineActor;


USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBPatrolInstanceData
{
	GENERATED_BODY()

	FCAIBPatrolInstanceData();

	UPROPERTY(EditAnywhere, Category=Context)
	TObjectPtr<ACharacter> Character;

	FCAIBBehaviorId BehaviorId;
};


USTRUCT(DisplayName="Common Patrol Task", Category="Common AI Behaviors")
struct COMMONAIBEHAVIORS_API FCAIBTask_Patrol : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FCAIBPatrolInstanceData;

	FCAIBTask_Patrol();
	

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
