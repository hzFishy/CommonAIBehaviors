// Copyright SurvivalMetroGame Dev Team. All Rights Reserved.


#include "Components/CAIBPerceptionStimuliSourceComponent.h"

#include "KismetTraceUtils.h"
#include "Data/CAIBTypes.h"


namespace CAIB::Debug
{
#if CAIB_WITH_DEBUG
	namespace Debug
	{
		FU_CMD_BOOL_TOGGLE(DebugSightCanBeSeenFrom,
			"CAIB.Debug.DebugSightCanBeSeenFrom", "",
			bDebugSightCanBeSeenFrom, false)
	}
#endif
}

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBPerceptionStimuliSourceComponent::UCAIBPerceptionStimuliSourceComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer),
	SightSphereSweepRadius(10)
{
	PrimaryComponentTick.bCanEverTick = true;
}

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
ECAIBVisibilityResult UCAIBPerceptionStimuliSourceComponent::CanBeSeenFrom(const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	// trace between AI head and owning actor "eyes" (if its a pawn this would typically be its head)
	// TODO: trace for other body parts such as center and feets.

	FHitResult Hit;

	FVector AILocationView;
	FRotator AIRotationView;
	Context.IgnoreActor->GetActorEyesViewPoint(AILocationView, AIRotationView);
	
	FVector TargetLocationView;
	FRotator TargetRotationView;
	GetOwner()->GetActorEyesViewPoint(TargetLocationView, TargetRotationView);
	
	FVector StartLocation = AILocationView;
	FVector EndLocation = TargetLocationView;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Context.IgnoreActor);
	
	const bool bHit = GetWorld()->SweepSingleByProfile(
		Hit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		SightTraceProfile.Name,
		FCollisionShape::MakeSphere(SightSphereSweepRadius),
		QueryParams
	);

#if CAIB_WITH_DEBUG
	if (CAIB::Debug::Debug::bDebugSightCanBeSeenFrom)
	{
		DrawDebugSweptSphere(
			GetWorld(),
			StartLocation,
			EndLocation,
			SightSphereSweepRadius,
			FColor::Red
		);
	}
#endif
	
	if (bHit)
	{
		if (Hit.GetActor() == GetOwner())
		{
			OutSeenLocation = EndLocation;
			OutSightStrength = 1;
			return ECAIBVisibilityResult::Visible;
		}
	}

	return ECAIBVisibilityResult::NotVisible;
}
