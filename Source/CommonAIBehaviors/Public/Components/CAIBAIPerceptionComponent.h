// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Perception/AIPerceptionComponent.h"
#include "CAIBAIPerceptionComponent.generated.h"


UCLASS(ClassGroup=(CommonAIBehaviors), DisplayName="Common AI Perception Component", meta=(BlueprintSpawnableComponent))
class COMMONAIBEHAVIORS_API UCAIBAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAIPerceptionComponent();

	virtual void OnRegister() override;

	virtual void OnUnregister() override;

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
protected:
	virtual void OnNewPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	virtual void OnInvalidPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	virtual void OnPerceptionSourceForgotten(AActor* ForgottenActor);
	
	
	/*----------------------------------------------------------------------------
		Callback
	----------------------------------------------------------------------------*/
protected:
	UFUNCTION() virtual void TargetPerceptionInfoUpdatedCallback(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	UFUNCTION() virtual void TargetPerceptionForgottenCallback(AActor* ForgottenActor);
};
