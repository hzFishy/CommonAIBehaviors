// By hzFishy - 2025 - Do whatever you want with it.


#include "Components/CAIBAIPerceptionComponent.h"

#include "Asserts/FUAsserts.h"
#include "Utility/FUUtilities.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBAIPerceptionComponent::UCAIBAIPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCAIBAIPerceptionComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD
#endif

	OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &ThisClass::TargetPerceptionInfoUpdatedCallback);
	OnTargetPerceptionForgotten.AddUniqueDynamic(this, &ThisClass::TargetPerceptionForgottenCallback);
}

void UCAIBAIPerceptionComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD
#endif

	OnTargetPerceptionInfoUpdated.RemoveDynamic(this, &ThisClass::TargetPerceptionInfoUpdatedCallback);
	OnTargetPerceptionForgotten.RemoveDynamic(this, &ThisClass::TargetPerceptionForgottenCallback);
}

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
void UCAIBAIPerceptionComponent::OnNewPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (auto* Entry = TrackedStimuliSources.Find(UpdateInfo.TargetId))
	{
		Entry->LatestStimulus = UpdateInfo.Stimulus;
	}
	else
	{
		TrackedStimuliSources.Add(UpdateInfo.TargetId, CAIBTrackedStimuliSource(UpdateInfo));
	}
}

void UCAIBAIPerceptionComponent::OnInvalidPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	
}

void UCAIBAIPerceptionComponent::OnPerceptionSourceForgotten(AActor* ForgottenActor)
{
	// get hash of Uobject to find it
	int32 TargetId = -1;
	auto* Entry = TrackedStimuliSources.Find(TargetId);
	if (FU_ENSURE(Entry))
	{
		
	}
}

	
	/*----------------------------------------------------------------------------
		Callback
	----------------------------------------------------------------------------*/
void UCAIBAIPerceptionComponent::TargetPerceptionInfoUpdatedCallback(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (UpdateInfo.Target.IsValid() && UpdateInfo.Stimulus.IsValid())
	{
		if (UpdateInfo.Stimulus.WasSuccessfullySensed())
		{
			OnNewPerceptionSource(UpdateInfo);
		}
		else
		{
			OnInvalidPerceptionSource(UpdateInfo);
		}
	}
}

void UCAIBAIPerceptionComponent::TargetPerceptionForgottenCallback(AActor* ForgottenActor)
{
	if (IsValid(ForgottenActor))
	{
		OnPerceptionSourceForgotten(ForgottenActor);
	}
}
