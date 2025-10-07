// By hzFishy - 2025 - Do whatever you want with it.


#include "Splines/CAIBPatrolSplineComponent.h"
#include "Splines/CAIBPatrolSplineMetadata.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBPatrolSplineComponent::UCAIBPatrolSplineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

/*
USplineMetadata* UCAIBPatrolSplineComponent::GetSplinePointsMetadata()
{
	if (auto* Actor = GetOwner<ACAIBPatrolSplineActor>())
	{
		return Actor->GetMutablePatrolSplineMetadata();
	}

	return nullptr;
}

const USplineMetadata* UCAIBPatrolSplineComponent::GetSplinePointsMetadata() const
{
	if (auto* Actor = GetOwner<ACAIBPatrolSplineActor>())
	{
		return Actor->GetPatrolSplineMetadata();
	}

	return nullptr;
}
*/

void UCAIBPatrolSplineComponent::PostLoad()
{
	Super::PostLoad();

	FixupPoints();
}

void UCAIBPatrolSplineComponent::PostDuplicate(bool bDuplicateForPie)
{
	Super::PostDuplicate(bDuplicateForPie);

	FixupPoints();
}

#if WITH_EDITOR
void UCAIBPatrolSplineComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FixupPoints();
}

void UCAIBPatrolSplineComponent::PostEditImport()
{
	Super::PostEditImport();

	FixupPoints();
}
#endif

void UCAIBPatrolSplineComponent::FixupPoints()
{
#if WITH_EDITORONLY_DATA
	// Keep metadata in sync
	if (auto* SplineMetadata = GetSplinePointsMetadata())
	{
		const int32 NumPoints = GetNumberOfSplinePoints();
		SplineMetadata->Fixup(NumPoints, this);
	}
#endif
}
