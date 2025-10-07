// By hzFishy - 2025 - Do whatever you want with it.


#include "Splines/CAIBPatrolSplineMetadata.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
/*
void UCAIBPatrolSplineMetadata::InsertPoint(int32 Index, float t, bool bClosedLoop)
{
	if (Index >= SplinePointsData.Num())
	{
		AddPoint(static_cast<float>(Index));
	}
	else
	{
		SplinePointsData.Insert(FCAIBPatrolSplinePointData(), Index);
	}

	Modify();
}

void UCAIBPatrolSplineMetadata::UpdatePoint(int32 Index, float t, bool bClosedLoop)
{
	Modify();
}

void UCAIBPatrolSplineMetadata::AddPoint(float InputKey)
{
	SplinePointsData.Emplace(FCAIBPatrolSplinePointData());
	Modify();
}

void UCAIBPatrolSplineMetadata::RemovePoint(int32 Index)
{
	SplinePointsData.RemoveAt(Index);
	Modify();
}

void UCAIBPatrolSplineMetadata::DuplicatePoint(int32 Index)
{
	FCAIBPatrolSplinePointData NewVal = SplinePointsData[Index];
	SplinePointsData.Insert(NewVal, Index);
	Modify();
}

void UCAIBPatrolSplineMetadata::CopyPoint(const USplineMetadata* FromSplineMetadata, int32 FromIndex, int32 ToIndex)
{
	if (const auto* FromMetadata = Cast<UCAIBPatrolSplineMetadata>(FromSplineMetadata))
	{
		SplinePointsData[ToIndex] = SplinePointsData[FromIndex];
		Modify();
	}
}

void UCAIBPatrolSplineMetadata::Reset(int32 NumPoints)
{
	SplinePointsData.Reset(NumPoints);
	Modify();
}

void UCAIBPatrolSplineMetadata::Fixup(int32 NumPoints, USplineComponent* SplineComp)
{
	if (SplinePointsData.Num() > NumPoints)
	{
		SplinePointsData.RemoveAt(NumPoints, SplinePointsData.Num() - NumPoints);
		Modify();
	}

	while (SplinePointsData.Num() < NumPoints)
	{
		SplinePointsData.Emplace(FCAIBPatrolSplinePointData());
		Modify();
	}
}
*/
