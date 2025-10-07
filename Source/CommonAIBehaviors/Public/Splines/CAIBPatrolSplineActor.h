// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CAIBPatrolSplineActor.generated.h"
class UCAIBPatrolSplineComponent;
class UCAIBPatrolSplineMetadata;


UCLASS(ClassGroup=CommonAIBehaviors, DisplayName="Common Patrol Spline Actor")
class COMMONAIBEHAVIORS_API ACAIBPatrolSplineActor : public AActor
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
protected:
	/*UPROPERTY(Instanced, Export)
	TObjectPtr<UCAIBPatrolSplineMetadata> PatrolSplineMetadata;*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCAIBPatrolSplineComponent> PatrolSplineComponent;

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	ACAIBPatrolSplineActor();

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	UCAIBPatrolSplineComponent* GetPatrolSplineComponent();
	
	/*UCAIBPatrolSplineMetadata* GetMutablePatrolSplineMetadata();
	
	const UCAIBPatrolSplineMetadata* GetPatrolSplineMetadata() const;*/
};
