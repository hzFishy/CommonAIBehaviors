// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBIdleTypes.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"

	
	/*----------------------------------------------------------------------------
		Static Idle
	----------------------------------------------------------------------------*/
FCAIBStaticIdleBaseRuntimeData::FCAIBStaticIdleBaseRuntimeData():
	CachedBaseData(nullptr),
	StartLocation(FVector::ZeroVector),
	StartRotation(FRotator::ZeroRotator),
	bMovingToStart(false)
{}

void FCAIBStaticIdleBaseRuntimeData::OnTargetActorSet()
{
	Super::OnTargetActorSet();

	CharacterAnimInstance = GetTargetCharacter()->GetMesh()->GetAnimInstance();
	Controller = GetTargetCharacter()->GetController<AAIController>();
	PathFollowingComponent = Controller->GetComponentByClass<UPathFollowingComponent>();
	StartLocation = GetTargetActor()->GetActorLocation();
	StartRotation = GetTargetActor()->GetActorRotation();
}

void FCAIBStaticIdleBaseRuntimeData::Pause()
{
	Super::Pause();

	StopCurrentAnimation();
}

void FCAIBStaticIdleBaseRuntimeData::Resume()
{
	Super::Resume();

	if (CachedBaseData->bOnResumeGoBackToStartLocation)
	{
		FAIMoveRequest MoveReq(StartLocation);
		MoveReq.SetUsePathfinding(true);
		MoveReq.SetAllowPartialPath(false);
		MoveReq.SetAcceptanceRadius(CachedBaseData->GoBackToStartAcceptanceRadius);
		MoveReq.SetReachTestIncludesAgentRadius(false);

		auto Result = Controller->MoveTo(MoveReq);
		if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
		{
			bMovingToStart = true;
			PathFinishDelegateHandle = PathFollowingComponent->OnRequestFinished.AddLambda(
			[this] (FAIRequestID RequestId, const FPathFollowingResult& Result)
				{
					PathFollowingComponent->OnRequestFinished.Remove(PathFinishDelegateHandle);

					if (CachedBaseData->bOnResumeGoBackToStartRotation)
					{
						// TODO: smooth it
						GetTargetActor()->SetActorRotation(StartRotation);
					}
					bMovingToStart = false;
					OnResumeFinished();
				}
			);
		}
	}
	else
	{
		OnResumeFinished();
	}
}

void FCAIBStaticIdleBaseRuntimeData::Stop()
{
	Super::Stop();

	StopCurrentAnimation();
}

void FCAIBStaticIdleBaseRuntimeData::OnResumeFinished()
{
	
}

void FCAIBStaticIdleBaseRuntimeData::StopCurrentAnimation()
{
	if (CharacterAnimInstance.IsValid() && CurrentMontage.IsValid())
	{
		CharacterAnimInstance->Montage_Stop(
			0.2,
			CurrentMontage.Get()
		);
	}
}


FCAIBStaticIdleSingleRuntimeData::FCAIBStaticIdleSingleRuntimeData():
	CachedSingleData(nullptr)
{}

void FCAIBStaticIdleSingleRuntimeData::Start()
{
	Super::Start();

	CachedSingleData = GetCachedDataAs<FCAIBStaticIdleBehaviorSingleData>();
	PlaySingleAnim();
}

void FCAIBStaticIdleSingleRuntimeData::Pause()
{
	Super::Pause();

	if (PathFinishDelegateHandle.IsValid())
	{
		PathFollowingComponent->OnRequestFinished.Remove(PathFinishDelegateHandle);
	}
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBStaticIdleSingleRuntimeData::GetDebugState() const
{
	if (!IsActive()) { return FFUMessageBuilder(); }

	if (bMovingToStart)
	{
		return FFUMessageBuilder()
			.Append("Single Anim Data")
		   .NewLine("Moving to start");
	}
	
	return FFUMessageBuilder()
		.Append("Single Anim Data")
		.NewLine("Montage: " + CurrentMontage->GetName());
}
#endif

void FCAIBStaticIdleSingleRuntimeData::OnResumeFinished()
{
	Super::OnResumeFinished();

	PlaySingleAnim();
}

void FCAIBStaticIdleSingleRuntimeData::PlaySingleAnim()
{
	auto* LoadedMontage = CachedSingleData->SoftMontage.LoadSynchronous();
	GetTargetCharacter()->PlayAnimMontage(LoadedMontage);
	CurrentMontage = LoadedMontage;
}


FCAIBStaticIdleSequenceRuntimeData::FCAIBStaticIdleSequenceRuntimeData():
	CachedSequenceData(nullptr),
	CurrentIndex(-1),
	TargetRelativeTime(0),
	ElapsedTime(0)
{}

void FCAIBStaticIdleSequenceRuntimeData::Start()
{
	Super::Start();

	CachedSequenceData = GetCachedDataAs<FCAIBStaticIdleBehaviorSequenceData>();
	
	GoNextSequenceEntry();
}

bool FCAIBStaticIdleSequenceRuntimeData::CanTick()
{
	return Super::CanTick() && !bMovingToStart;
}

void FCAIBStaticIdleSequenceRuntimeData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	if (ElapsedTime >= TargetRelativeTime)
	{
		GoNextSequenceEntry();
	}
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBStaticIdleSequenceRuntimeData::GetDebugState() const
{
	if (!IsActive()) { return FFUMessageBuilder(); }

	if (bMovingToStart)
	{
		return FFUMessageBuilder()
		   .Append("Sequence Anim Data")
		   .NewLine("Moving to start");
	}
	
	return FFUMessageBuilder()
	   .Append("Sequence Anim Data")
	   .NewLine(TEXT("CurrentIndex: %i"), CurrentIndex)
	   .NewLine("Current Montage: " + CurrentMontage->GetName())
	   .NewLine(TEXT("%.1fs/%.1fs"), ElapsedTime, TargetRelativeTime);
}
#endif

void FCAIBStaticIdleSequenceRuntimeData::OnResumeFinished()
{
	Super::OnResumeFinished();

	if (CachedSequenceData->bOnResumeGoBackToInitialIndex)
	{
		CurrentIndex = -1;
	}
	if (CachedSequenceData->bOnResumeResetAnimationProgress)
	{
		ElapsedTime = 0;
	}
	
	GoNextSequenceEntry();
}

int32 FCAIBStaticIdleSequenceRuntimeData::GetNextSequenceEntryIndex()
{
	switch (CachedSequenceData->SelectionType)
	{
		case ECAIBStaticIdleBehaviorSequenceType::InOrder:
			{
				// simply get next valid index for array
				int32 NextIndex = CurrentIndex + 1;
				if (!CachedSequenceData->SequenceEntries.IsValidIndex(NextIndex))
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
					return FMath::RandRange(0, CachedSequenceData->SequenceEntries.Num() - 1);
				}

				int32 NewIndex = FMath::RandRange(0, CachedSequenceData->SequenceEntries.Num() - 2);
				if (NewIndex >= CurrentIndex)
				{
					NewIndex++;
				}
				return NewIndex;
			}
		case ECAIBStaticIdleBehaviorSequenceType::RandomAll:
			{
				// get a random index
				return FMath::RandRange(0, CachedSequenceData->SequenceEntries.Num() - 1);
			}
	}

	checkNoEntry()
	return -1;
}

void FCAIBStaticIdleSequenceRuntimeData::GoNextSequenceEntry()
{
	// select new index and get linked entry
	CurrentIndex = GetNextSequenceEntryIndex();
	ExecuteCurrentSequenceEntry();
}

void FCAIBStaticIdleSequenceRuntimeData::ExecuteCurrentSequenceEntry()
{
	const auto& SequenceEntryData = CachedSequenceData->SequenceEntries[CurrentIndex];

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


FCAIBStaticIdleBehaviorBaseData::FCAIBStaticIdleBehaviorBaseData():
	bOnResumeGoBackToStartLocation(true),
	bOnResumeGoBackToStartRotation(true),
	GoBackToStartAcceptanceRadius(50)
{}

FCAIBStaticIdleBehaviorBaseData::~FCAIBStaticIdleBehaviorBaseData() {}

TSharedPtr<FCAIBStaticIdleBaseRuntimeData> FCAIBStaticIdleBehaviorBaseData::MakeSharedRuntime() const
{
	unimplemented();
	return TSharedPtr<FCAIBStaticIdleBaseRuntimeData>();
}


FCAIBStaticIdleBehaviorSingleData::FCAIBStaticIdleBehaviorSingleData() {}

TSharedPtr<FCAIBStaticIdleBaseRuntimeData> FCAIBStaticIdleBehaviorSingleData::MakeSharedRuntime() const
{
	return MakeShared<FCAIBStaticIdleSingleRuntimeData>();
}


FCAIBStaticIdleBehaviorSequenceEntryData::FCAIBStaticIdleBehaviorSequenceEntryData():
	MontagePlayRate(1)
{}

FCAIBStaticIdleBehaviorSequenceData::FCAIBStaticIdleBehaviorSequenceData():
	SelectionType(ECAIBStaticIdleBehaviorSequenceType::InOrder),
	bOnResumeGoBackToInitialIndex(false),
	bOnResumeResetAnimationProgress(true)
{}

TSharedPtr<FCAIBStaticIdleBaseRuntimeData> FCAIBStaticIdleBehaviorSequenceData::MakeSharedRuntime() const
{
	return MakeShared<FCAIBStaticIdleSequenceRuntimeData>();
}

FCAIBAIBehaviorStaticIdleFragment::FCAIBAIBehaviorStaticIdleFragment() {}
