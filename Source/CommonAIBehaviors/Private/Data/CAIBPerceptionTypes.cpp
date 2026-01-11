// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBPerceptionTypes.h"
#include "Components/CAIBAIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISense_Blueprint.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Prediction.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Team.h"
#include "Perception/AISense_Touch.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_SIGHT, "AI.Senses.Sight");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_HEARING, "AI.Senses.Hearing");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_TOUCH, "AI.Senses.Touch");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_DAMAGE, "AI.Senses.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_TEAM, "AI.Senses.Team");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_PREDICTION, "AI.Senses.Prediction");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_SENSES_BLUEPRINT, "AI.Senses.Blueprint");


FGameplayTag ICAIBSenseInterface::GetSenseTag() const
{
	return FGameplayTag();
}

FGameplayTag ICAIBSenseInterface::StaticGetSenseTag(const UObject* Object)
{
	if (Object)
	{
		if (Object->IsA(UAISense_Sight::StaticClass()))
		{
			return TAG_AI_SENSES_SIGHT;
		}
		else if (Object->IsA(UAISense_Hearing::StaticClass()))
		{
			return TAG_AI_SENSES_HEARING;
		}
		else if (Object->IsA(UAISense_Touch::StaticClass()))
		{
			return TAG_AI_SENSES_TOUCH;
		}
		else if (Object->IsA(UAISense_Prediction::StaticClass()))
		{
			return TAG_AI_SENSES_PREDICTION;
		}
		else if (Object->IsA(UAISense_Damage::StaticClass()))
		{
			return TAG_AI_SENSES_DAMAGE;
		}
		else if (Object->IsA(UAISense_Blueprint::StaticClass()))
		{
			return TAG_AI_SENSES_BLUEPRINT;
		}
		else if (Object->IsA(UAISense_Team::StaticClass()))
		{
			return TAG_AI_SENSES_TEAM;
		}
	}
	
	return FGameplayTag();
}



FCAIBAIStimulus::FCAIBAIStimulus():
	Strength(0),
	StimulusLocation(FVector::ZeroVector),
	ReceiverLocation(FVector::ZeroVector),
	Tag(NAME_None),
	Type(FAISenseID::InvalidID()),
	MaxAge(0),
	CurrentAge(0),
	bSuccessfullySensed(0),
	bExpired(0)
{}

FCAIBAIStimulus::FCAIBAIStimulus(const FAIStimulus& Stimulus):
	Strength(Stimulus.Strength),
	StimulusLocation(Stimulus.StimulusLocation),
	ReceiverLocation(Stimulus.ReceiverLocation),
	Tag(Stimulus.Tag),
	Type(Stimulus.Type),
	MaxAge(0),
	CurrentAge(Stimulus.GetAge()),
	bSuccessfullySensed(Stimulus.WasSuccessfullySensed()),
	bExpired(Stimulus.IsExpired())
{}

FCAIBAIStimulus::FCAIBAIStimulus(const FCAIBAIStimulus& PreviousStim, const FAIStimulus& NewStimulus):
	Strength(NewStimulus.Strength),
	StimulusLocation(NewStimulus.StimulusLocation),
	ReceiverLocation(NewStimulus.ReceiverLocation),
	Tag(NewStimulus.Tag),
	Type(NewStimulus.Type),
	MaxAge(PreviousStim.MaxAge),
	CurrentAge(NewStimulus.GetAge()),
	bSuccessfullySensed(NewStimulus.WasSuccessfullySensed()),
	bExpired(NewStimulus.IsExpired())
{}

FCAIBTrackedSensesContainer::FCAIBTrackedSensesContainer() {}

FCAIBTrackedSensesContainer::FCAIBTrackedSensesContainer(UCAIBAIPerceptionComponent* Comp, const FActorPerceptionUpdateInfo& UpdateInfo)
{
	PerceptionComponent = Comp;
	AddOrUpdateSense(UpdateInfo);
}

void FCAIBTrackedSensesContainer::AddOrUpdateSense(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	auto* Config = PerceptionComponent->GetSenseConfig(UpdateInfo.Stimulus.Type);
	auto* Sense = Config->GetSenseImplementation()->GetDefaultObject<UAISense>();

	FGameplayTag FoundSenseTag;
	if (CachedSensesTag.Contains(Sense))
	{
		FoundSenseTag = CachedSensesTag[Sense];
	}
	else
	{
		if (auto* SenseInterface = Cast<ICAIBSenseInterface>(Sense))
		{
			FoundSenseTag = SenseInterface->GetSenseTag();
		}
		else
		{
			FoundSenseTag = ICAIBSenseInterface::StaticGetSenseTag(Sense);
		}

		CachedSensesTag.Add(Sense, FoundSenseTag);
	}
	
	if (auto* Entry = PerSenseSources.Find(FoundSenseTag))
	{
		auto PreviousStimulus = Entry->LatestStimulus;
		Entry->LatestStimulus = FCAIBAIStimulus(PreviousStimulus, UpdateInfo.Stimulus);

		PerceptionComponent->OnUpdatedSensePerceivedForTargetDelegate.Broadcast(
			PerceptionComponent.Get(),
			FoundSenseTag,
			*Entry
		);
	}
	else
	{
		// init max age here
		auto NewStimulus = FCAIBTrackedStimulusSource(UpdateInfo);
		NewStimulus.LatestStimulus.MaxAge = Config->GetMaxAge();
		
		auto& Value = PerSenseSources.Add(FoundSenseTag, NewStimulus);
		PerceptionComponent->OnNewSensePerceivedForTargetDelegate.Broadcast(
			PerceptionComponent.Get(),
			FoundSenseTag,
			Value
		);
	}
}

void FCAIBTrackedSensesContainer::RemoveSense(FGameplayTag SenseTag)
{
	FCAIBTrackedStimulusSource RemovedValue;
	PerSenseSources.RemoveAndCopyValue(SenseTag, RemovedValue);

	PerceptionComponent->OnForgottenSensePerceivedForTargetDelegate.Broadcast(
		PerceptionComponent.Get(),
		SenseTag
	);

	// remove stimuli in the perception system
	if (RemovedValue.Target.IsValid())
	{
		PerceptionComponent->ForgetActor(RemovedValue.Target.Get());
	}
}


FCAIBTrackedStimulusSource::FCAIBTrackedStimulusSource():
	TargetId(-1)
{}

FCAIBTrackedStimulusSource::FCAIBTrackedStimulusSource(const FActorPerceptionUpdateInfo& UpdateInfo):
	TargetId(UpdateInfo.TargetId),
	Target(UpdateInfo.Target),
	LatestStimulus(UpdateInfo.Stimulus)
{}
