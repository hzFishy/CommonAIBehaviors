// Copyright SurvivalMetroGame Dev Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CAIBAIBehaviorObject.generated.h"


/**
 * Special object behavior that can be injected in a array on the behavior subsystem.
 */
UCLASS(Abstract)
class COMMONAIBEHAVIORS_API UCAIBAIBehaviorObject : public UObject
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
protected:
	float ElapsedTime;
	bool bFinished;

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAIBehaviorObject();

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	virtual void Start();

	virtual void Tick(float DeltaTime);
	
	virtual void Stop();

	UFUNCTION(BlueprintCallable, Category="Common AI Behaviors")
	virtual void MarkAsFinished();
	
	virtual bool IsMarkedAsFinished() const;
};
