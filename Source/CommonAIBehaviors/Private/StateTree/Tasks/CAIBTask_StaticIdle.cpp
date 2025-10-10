// By hzFishy - 2025 - Do whatever you want with it.


#include "StateTree/Tasks/CAIBTask_StaticIdle.h"
#include "StateTreeExecutionContext.h"
#include "Asserts/FUAsserts.h"
#include "Components/CAIBAIBehaviorComponent.h"
#include "Core/CAIBBehaviorSubsystem.h"
#include "Data/CAIBIdleTypes.h"
#include "GameFramework/Character.h"


FCAIBStaticIdleInstanceData::FCAIBStaticIdleInstanceData() {}

FCAIBTask_StaticIdle::FCAIBTask_StaticIdle()
{
	bShouldCallTick = false;
}

	
	/*----------------------------------------------------------------------------
		Defauls
	----------------------------------------------------------------------------*/
EStateTreeRunStatus FCAIBTask_StaticIdle::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& InstanceData = Context.GetInstanceData(*this);

	if (!FU_ENSURE_VALID(InstanceData.Character.Get()))
	{
		return EStateTreeRunStatus::Failed;
	}

	auto* AIBehaviorComponent = InstanceData.Character->FindComponentByClass<UCAIBAIBehaviorComponent>();

	if (!FU_ENSURE_VALID(AIBehaviorComponent))
	{
		return EStateTreeRunStatus::Failed;
	}

	auto* StaticIdleFragment = AIBehaviorComponent->GetAIBehaviorFragment<FCAIBAIBehaviorStaticIdleFragment>();

	if (!FU_ENSURE(StaticIdleFragment))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	InstanceData.CharacterAnimInstance = InstanceData.Character->GetMesh()->GetAnimInstance();

	if (!FU_ENSURE_WEAKVALID(InstanceData.CharacterAnimInstance))
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
		auto* Fragment =  StaticIdleFragment->Data.GetPtr<FCAIBStaticIdleBehaviorBaseData>();
		// make runtime struct
		auto RuntimeIdleData = Fragment->MakeSharedRuntime();

		// send it to the subsystem
		RuntimeIdleData->SetTargetActor(InstanceData.Character.Get());
		RuntimeIdleData->CachedBaseData = Fragment;
		InstanceData.BehaviorId = BehaviorSubsystem->AddActiveBehavior(RuntimeIdleData);
		
		BehaviorSubsystem->CacheNewBehaviorid(
			FCAIBStateTreeCacheId(Context),
			InstanceData.BehaviorId
		);
	}
	
	return EStateTreeRunStatus::Running;
}

void FCAIBTask_StaticIdle::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& InstanceData = Context.GetInstanceData(*this);

	auto* BehaviorSubsystem = Context.GetWorld()->GetSubsystem<UCAIBBehaviorSubsystem>();
	if (FU_ENSURE_VALID(BehaviorSubsystem))
	{
		// put to "sleep" the behavior, will take it back when this state will be re-entered
		BehaviorSubsystem->PauseBehavior(InstanceData.BehaviorId);
	}
}
