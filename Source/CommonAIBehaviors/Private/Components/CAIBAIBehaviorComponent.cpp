// By hzFishy - 2025 - Do whatever you want with it.


#include "Components/CAIBAIBehaviorComponent.h"
#if WITH_EDITORONLY_DATA
#include "Data/CAIBPatrolTypes.h"
#include "Splines/CAIBPatrolSplineActor.h"
#include "Splines/CAIBPatrolSplineComponent.h"
#endif

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBAIBehaviorComponent::UCAIBAIBehaviorComponent():
	bBehaviorsEnabled(true)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCAIBAIBehaviorComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITORONLY_DATA
	if (auto* PatrolFrag = GetMutableAIBehaviorFragment<FCAIBAIBehaviorPatrolFragment>())
	{
		if (auto* SplinePatrolFrag = PatrolFrag->Data.GetMutablePtr<FCAIBAIBehaviorPatrolSplineData>())
		{
			if (!SplineRenderUpdateHandle.IsValid())
			{
				SplineRenderUpdateHandle = UActorComponent::MarkRenderStateDirtyEvent.AddLambda([this, SplinePatrolFrag] (UActorComponent& UpdatedComp)
				{
					if (!UpdatedComp.IsA<UCAIBPatrolSplineComponent>())
					{
						return;
					}
					
					if (!SplinePatrolFrag->PatrolSplineActor.IsNull()
						&& SplinePatrolFrag->PatrolSplineActor.IsValid()
						&& SplinePatrolFrag->PatrolSplineActor->GetPatrolSplineComponent() == &UpdatedComp)
					{
						SplinePatrolFrag->ResfreshStatus();
					}
				});
			}
		}
	}
#endif
}

void UCAIBAIBehaviorComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITORONLY_DATA
	if (SplineRenderUpdateHandle.IsValid())
	{
		UActorComponent::MarkRenderStateDirtyEvent.Remove(SplineRenderUpdateHandle);
	}
#endif
}
