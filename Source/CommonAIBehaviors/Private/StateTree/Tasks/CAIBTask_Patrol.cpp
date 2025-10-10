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

	// check if we ran this previously
	// we need to get the behavior id, previously linked to that object

	auto* CachedId = BehaviorSubsystem->GetCachedBehavioridSafe(FCAIBStateTreeCacheId(Context));
	if (CachedId)
	{
		InstanceData.BehaviorId = *CachedId;
	}

	const bool bDidAlreadyRun = BehaviorSubsystem->HasBehavior(InstanceData.BehaviorId);

	if (bDidAlreadyRun)
	{
		BehaviorSubsystem->ResumeBehavior(InstanceData.BehaviorId);
	}
	else
	{
		auto SharedData = PatrolFragment->Data.GetPtr<FCAIBAIBehaviorPatrolBaseData>()->MakeSharedRuntime();
		SharedData->SetTargetActor(InstanceData.Character.Get());
		SharedData->CachedPatrolFragment = PatrolFragment;
		InstanceData.BehaviorId = BehaviorSubsystem->AddActiveBehavior(SharedData);

		BehaviorSubsystem->CacheNewBehaviorid(
			FCAIBStateTreeCacheId(Context),
			InstanceData.BehaviorId
		);
	}

	
	return EStateTreeRunStatus::Running;
}

void FCAIBTask_Patrol::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.BehaviorId.IsValid())
	{
		if (auto* BehaviorSubsystem = Context.GetWorld()->GetSubsystem<UCAIBBehaviorSubsystem>())
		{
			// put to "sleep" the behavior, will take it back when this state will be re-entered
			BehaviorSubsystem->PauseBehavior(InstanceData.BehaviorId);
		}
	}
}
