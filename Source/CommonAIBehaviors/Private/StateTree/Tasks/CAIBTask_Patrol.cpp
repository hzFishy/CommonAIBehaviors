// By hzFishy - 2025 - Do whatever you want with it.


#include "StateTree/Tasks/CAIBTask_Patrol.h"
#include "StateTreeExecutionContext.h"
#include "Asserts/FUAsserts.h"
#include "Components/CAIBAIBehaviorComponent.h"
#include "Core/CAIBBehaviorSubsystem.h"
#include "Data/CAIBPatrolTypes.h"
#include "GameFramework/Character.h"
#include "Splines/CAIBPatrolSplineActor.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
FCAIBPatrolInstanceData::FCAIBPatrolInstanceData() {}


FCAIBTask_Patrol::FCAIBTask_Patrol()
{
	bShouldCallTick = false;
}

EStateTreeRunStatus FCAIBTask_Patrol::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& InstanceData = Context.GetInstanceData(*this);

	if (!FU_ENSURE_VALID(InstanceData.Character))
	{
		return EStateTreeRunStatus::Failed;
	}

	auto* AIBehaviorComponent = InstanceData.Character->FindComponentByClass<UCAIBAIBehaviorComponent>();

	if (!FU_ENSURE_VALID(AIBehaviorComponent))
	{
		return EStateTreeRunStatus::Failed;
	}

	auto* PatrolFragment = AIBehaviorComponent->GetAIBehaviorFragment<FCAIBAIBehaviorPatrolFragment>();
	if (!FU_ENSURE(PatrolFragment))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	auto* BehaviorSubsystem = Context.GetWorld()->GetSubsystem<UCAIBBehaviorSubsystem>();
	if (!FU_ENSURE(BehaviorSubsystem))
	{
		return EStateTreeRunStatus::Failed;
	}

	auto SharedData = PatrolFragment->Data.GetPtr<FCAIBAIBehaviorPatrolBaseData>()->MakeSharedRuntime();
	SharedData->SetTargetActor(InstanceData.Character.Get());
	SharedData->CachedPatrolFragment = PatrolFragment;
	InstanceData.BehaviorId = BehaviorSubsystem->AddActiveBehavior(SharedData);
	
	return EStateTreeRunStatus::Running;
}

void FCAIBTask_Patrol::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.BehaviorId.IsValid())
	{
		if (auto* BehaviorSubsystem = Context.GetWorld()->GetSubsystem<UCAIBBehaviorSubsystem>())
		{
			BehaviorSubsystem->StopAndRemoveBehavior(InstanceData.BehaviorId);
		}
	}
}
