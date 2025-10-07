// By hzFishy - 2025 - Do whatever you want with it.


#include "Splines/CAIBPatrolSplineActor.h"
#include "Splines/CAIBPatrolSplineComponent.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
ACAIBPatrolSplineActor::ACAIBPatrolSplineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	/*
	PatrolSplineMetadata = CreateDefaultSubobject<UCAIBPatrolSplineMetadata>("PatrolSplineMetadata");
	PatrolSplineMetadata->Reset(2);
	PatrolSplineMetadata->AddPoint(0.0f);
	PatrolSplineMetadata->AddPoint(1.0f);
	*/

	PatrolSplineComponent = CreateDefaultSubobject<UCAIBPatrolSplineComponent>("PatrolSplineComponent");
	SetRootComponent(PatrolSplineComponent);
}

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
UCAIBPatrolSplineComponent* ACAIBPatrolSplineActor::GetPatrolSplineComponent()
{
	return PatrolSplineComponent.Get();
}

/*
UCAIBPatrolSplineMetadata* ACAIBPatrolSplineActor::GetMutablePatrolSplineMetadata()
{
	return PatrolSplineMetadata.Get();
}

const UCAIBPatrolSplineMetadata* ACAIBPatrolSplineActor::GetPatrolSplineMetadata() const
{
	return PatrolSplineMetadata.Get();
}
*/
