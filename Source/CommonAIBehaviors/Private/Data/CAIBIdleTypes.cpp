// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBIdleTypes.h"
#include "GameFramework/Character.h"

	
	/*----------------------------------------------------------------------------
		Static Idle
	----------------------------------------------------------------------------*/
FCAIBStaticIdleBehaviorBaseData::FCAIBStaticIdleBehaviorBaseData()
{
}


FCAIBStaticIdleBehaviorSingleData::FCAIBStaticIdleBehaviorSingleData()
{
}


FCAIBStaticIdleBehaviorSequenceEntryData::FCAIBStaticIdleBehaviorSequenceEntryData():
	MontagePlayRate(1)
{
}

FCAIBStaticIdleBehaviorSequenceData::FCAIBStaticIdleBehaviorSequenceData():
	SelectionType(ECAIBStaticIdleBehaviorSequenceType::InOrder)
{
}

FCAIBAIBehaviorStaticIdleFragment::FCAIBAIBehaviorStaticIdleFragment()
{
}


FCAIBStaticIdleRuntimeData::FCAIBStaticIdleRuntimeData():
	SequenceData(nullptr),
	CurrentIndex(-1),
	TargetRelativeTime(0),
	ElapsedTime(0)
{
}

void FCAIBStaticIdleRuntimeData::OnTargetActorSet()
{
	Super::OnTargetActorSet();

	CharacterAnimInstance = GetTargetCharacter()->GetMesh()->GetAnimInstance();
}

void FCAIBStaticIdleRuntimeData::Start()
{
	Super::Start();

	GoNextSequenceEntry();
}

void FCAIBStaticIdleRuntimeData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	if (ElapsedTime >= TargetRelativeTime)
	{
		GoNextSequenceEntry();
	}
}

void FCAIBStaticIdleRuntimeData::Stop()
{
	Super::Stop();

	// abort sequence
	if (CharacterAnimInstance.IsValid() && CurrentMontage.IsValid())
	{
		CharacterAnimInstance->Montage_Stop(
			0.2,
			CurrentMontage.Get()
		);
	}
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBStaticIdleRuntimeData::GetDebugState() const
{
	return FFUMessageBuilder()
	       .Append("Sequence Anim Data")
	       .NewLine(TEXT("CurrentIndex: %i"), CurrentIndex)
	       .NewLine("CurrentMontage: " + CurrentMontage->GetName())
	       .NewLine(TEXT("%.1fs/%.1fs"), ElapsedTime, TargetRelativeTime);
}
#endif

int32 FCAIBStaticIdleRuntimeData::GetNextSequenceEntryIndex()
{
	switch (SequenceData->SelectionType)
	{
	case ECAIBStaticIdleBehaviorSequenceType::InOrder:
		{
			// simply get next valid index for array
			int32 NextIndex = CurrentIndex + 1;
			if (!SequenceData->SequenceEntries.IsValidIndex(NextIndex))
			{
				NextIndex = 0;
			}
			return NextIndex;
		}
	case ECAIBStaticIdleBehaviorSequenceType::RandomExcludeSelf:
		{
			// get a random index but exclude the current index

			if (CurrentIndex < 0)
			{
				return FMath::RandRange(0, SequenceData->SequenceEntries.Num() - 1);
			}

			int32 NewIndex = FMath::RandRange(0, SequenceData->SequenceEntries.Num() - 2);
			if (NewIndex >= CurrentIndex)
			{
				NewIndex++;
			}
			return NewIndex;
		}
	case ECAIBStaticIdleBehaviorSequenceType::RandomAll:
		{
			// get a random index
			return FMath::RandRange(0, SequenceData->SequenceEntries.Num() - 1);
		}
	}

	checkNoEntry()
	return -1;
}

void FCAIBStaticIdleRuntimeData::GoNextSequenceEntry()
{
	// select new index and get linked entry
	CurrentIndex = GetNextSequenceEntryIndex();

	const auto& SequenceEntryData = SequenceData->SequenceEntries[CurrentIndex];

	// TODO (perf): async load montage
	CurrentMontage = SequenceEntryData.SoftMontage.LoadSynchronous();

	// play anim
	const float MontageDuration = CharacterAnimInstance->Montage_Play(
		CurrentMontage.Get(),
		SequenceEntryData.MontagePlayRate,
		EMontagePlayReturnType::Duration
	);

	// now we need to know when we have to go to the next animation
	TargetRelativeTime = SequenceEntryData.Duration.IsSet() ? SequenceEntryData.Duration.GetValue() : MontageDuration;
	// reset time
	ElapsedTime = 0;
}

	