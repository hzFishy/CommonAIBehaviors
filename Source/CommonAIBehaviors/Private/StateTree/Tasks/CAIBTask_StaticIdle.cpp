// By hzFishy - 2025 - Do whatever you want with it.


#include "StateTree/Tasks/CAIBTask_StaticIdle.h"
#include "StateTreeExecutionContext.h"
#include "Asserts/FUAsserts.h"
#include "Components/CAIBAIBehaviorComponent.h"
#include "Core/CAIBBehaviorSubsystem.h"
#include "Data/CAIBIdleTypes.h"
#include "GameFramework/Character.h"


FCAIBStaticIdleInstanceData::FCAIBStaticIdleInstanceData():
	DebugId(0)
{}

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
	
	// start the phase
	if (auto* SingleData = StaticIdleFragment->Data.GetPtr<FCAIBStaticIdleBehaviorSingleData>())
	{
		// just play and dont care about any other runtime stuff
		// TODO (perf): async load montage
		InstanceData.CurrentSingleMontage = SingleData->SoftMontage.LoadSynchronous();
		InstanceData.Character->PlayAnimMontage(InstanceData.CurrentSingleMontage.Get());

		FCAIBAIBehaviorDebugMessageEntry Entry;
		Entry.TargetActor = InstanceData.Character.Get();
		Entry.MessageBuilder
			.Append("Single Anim Data")
			.NewLine("CurrentMontage: " + InstanceData.CurrentSingleMontage->GetName());
		InstanceData.DebugId = BehaviorSubsystem->AddDebugMessage(Entry);
	}
	else if (auto* SequenceData = StaticIdleFragment->Data.GetPtr<FCAIBStaticIdleBehaviorSequenceData>())
	{
		auto* Character = InstanceData.Character.Get();
		
		// send it to the subsystem
		auto SharedData = MakeShared<FCAIBStaticIdleRuntimeData>();
		SharedData->SetTargetActor(Character);
		SharedData->SequenceData = SequenceData;
		InstanceData.SequenceBehaviorId = BehaviorSubsystem->AddBehavior(SharedData);
	}
	
	return EStateTreeRunStatus::Running;
}

void FCAIBTask_StaticIdle::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& InstanceData = Context.GetInstanceData(*this);

	auto* BehaviorSubsystem = Context.GetWorld()->GetSubsystem<UCAIBBehaviorSubsystem>();
	if (FU_ENSURE_VALID(BehaviorSubsystem))
	{
		if (InstanceData.CurrentSingleMontage.IsValid())
		{
			// for single sequence just stop it
			InstanceData.CharacterAnimInstance->Montage_Stop(
				0.2,
				InstanceData.CurrentSingleMontage.Get()
			);
			BehaviorSubsystem->RemoveDebugMessage(InstanceData.DebugId);
		}
		else if (InstanceData.SequenceBehaviorId.IsValid())
		{
			// to abort sequence we let the subsystem do it
			BehaviorSubsystem->StopAndRemoveBehavior(InstanceData.SequenceBehaviorId);
		}
	}
}
