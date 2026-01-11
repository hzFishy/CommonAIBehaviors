// Copyright SurvivalMetroGame Dev Team. All Rights Reserved.

#pragma once

#include "Data/CAIBTypes.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISightTargetInterface.h"
#include "CAIBPerceptionStimuliSourceComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMMONAIBEHAVIORS_API UCAIBPerceptionStimuliSourceComponent : public UAIPerceptionStimuliSourceComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
protected:
	UPROPERTY(EditAnywhere, Category="Common AI Behaviors|Sight")
	float SightSphereSweepRadius;
	
	UPROPERTY(EditAnywhere, Category="Common AI Behaviors|Sight")
	FCollisionProfileName SightTraceProfile;

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBPerceptionStimuliSourceComponent(const FObjectInitializer& ObjectInitializer);

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	/**
	 *  Meants to be used with IAISightTargetInterface on the owning Pawn
	 */
	virtual ECAIBVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation,
		int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength,
		int32* UserData = nullptr, const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr);
};
