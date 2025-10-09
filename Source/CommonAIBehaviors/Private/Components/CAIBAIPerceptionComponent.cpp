// By hzFishy - 2025 - Do whatever you want with it.


#include "Components/CAIBAIPerceptionComponent.h"
#include "Asserts/FUAsserts.h"
#include "Utility/FUUtilities.h"


FCAIBStimuliExpired::FCAIBStimuliExpired():
	SourceId(0)
{}

FCAIBStimuliExpired::FCAIBStimuliExpired(int32 InId, FGameplayTag InTag):
	SourceId(InId),
	SenseTag(InTag)
{}

	
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
}

void UCAIBAIPerceptionComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD
#endif

	OnTargetPerceptionInfoUpdated.RemoveDynamic(this, &ThisClass::TargetPerceptionInfoUpdatedCallback);
}

void UCAIBAIPerceptionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickTrackedSources(DeltaTime);
}

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
void UCAIBAIPerceptionComponent::OnNewPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (auto* Entry = TrackedSources.Find(UpdateInfo.TargetId))
	{
		Entry->AddOrUpdateSense(UpdateInfo);
	}
	else
	{
		TrackedSources.Add(UpdateInfo.TargetId, FCAIBTrackedSensesContainer(this, UpdateInfo));
	}
}

void UCAIBAIPerceptionComponent::OnInvalidPerceptionSource(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	auto* Entry = TrackedSources.Find(UpdateInfo.TargetId);
	if (FU_ENSURE(Entry))
	{
		Entry->AddOrUpdateSense(UpdateInfo);
	}
}

void UCAIBAIPerceptionComponent::OnPerceptionSourceForgotten(int32 SourceId, FGameplayTag SenseTag)
{
	ExpiredSources.Emplace(FCAIBStimuliExpired(SourceId, SenseTag));
}

void UCAIBAIPerceptionComponent::TickTrackedSources(float DeltaTime)
{
	if (TrackedSources.IsEmpty()) { return; }
	
	ExpiredSources.Reserve(TrackedSources.Num());
	
	// age stimuli
	for (auto& TrackedPair : TrackedSources)
	{
		auto& Container = TrackedPair.Value;
		
		for (auto& StimuliPair : Container.GetMutableMap())
		{
			auto& StimSenseTag = StimuliPair.Key;
			auto& StimData = StimuliPair.Value;

			StimData.LatestStimulus.CurrentAge += DeltaTime;
			if (StimData.LatestStimulus.IsExpired())
			{
				OnPerceptionSourceForgotten(StimData.TargetId, StimSenseTag);
			}
		}
	}

	// removed expired stims
	for (auto& ExpiredSource : ExpiredSources)
	{
		TrackedSources[ExpiredSource.SourceId].RemoveSense(ExpiredSource.SenseTag);
	}

	ExpiredSources.Empty();
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
