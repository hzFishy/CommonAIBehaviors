// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Components/SplineComponent.h"
#include "CAIBPatrolSplineComponent.generated.h"



UCLASS(ClassGroup=CommonAIBehaviors, DisplayName="Common Patrol Spline Component", meta=(BlueprintSpawnableComponent))
class COMMONAIBEHAVIORS_API UCAIBPatrolSplineComponent : public USplineComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBPatrolSplineComponent();

	//virtual USplineMetadata* GetSplinePointsMetadata() override;
	//virtual const USplineMetadata* GetSplinePointsMetadata() const override;
	
	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPie) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditImport() override;
#endif
	
	void FixupPoints();
};
