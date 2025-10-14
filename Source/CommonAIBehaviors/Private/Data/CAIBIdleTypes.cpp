// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBIdleTypes.h"

#include "AIController.h"
#include "Asserts/FUAsserts.h"
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

	if (CachedSingleData->SoftMontage.IsNull())
	{
		return;
	}

	if (CachedBaseData->bAsyncLoadBeforeStarting && !CachedSingleData->SoftMontage.IsValid())
	{
		CachedSingleData->SoftMontage.LoadAsync(
			FLoadSoftObjectPathAsyncDelegate::CreateLambda([this] (const FSoftObjectPath&, UObject* LoadedObj)
			{
				FU_ENSURE_VALID(LoadedObj);
				PlaySingleAnim();
			})
		);
	}
	else
	{
		PlaySingleAnim();
	}
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

	if (CachedSingleData->SoftMontage.IsNull())
	{
		return FFUMessageBuilder("Montage is not set");
	}
	
	if (bMovingToStart)
	{
		return FFUMessageBuilder()
			.Append("Single Anim Data")
		   .NewLine("Moving to start");
	}

	if (!CurrentMontage.IsValid())
	{
		return FFUMessageBuilder("Montage is null, may be loading");
	}
	
	return FFUMessageBuilder()
		.Append("Single Anim Data")
		.NewLine("Montage: " + CurrentMontage->GetName());
}
#endif

void FCAIBStaticIdleSingleRuntimeData::OnResumeFinished()
{
	Super::OnResumeFinished();

	// race condition if this is called while the async request (if one) wasnt finished
	// this shouldnt happen because an animation is fast to load
	// and perception should not happen until at least a few secs
	PlaySingleAnim();
}

void FCAIBStaticIdleSingleRuntimeData::PlaySingleAnim()
{
	// we call load sync to be sure we got something to call,
	// but if bAsyncLoadBeforeStarting is true this will not load anything
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
	
	// either load everything as defined or load only first anim, or nothing
	if (CachedSequenceData->AsyncLoadType == ECAIBStaticIdleSequencerAsyncLoadType::AsyncLoadAllBeforeStart)
	{
		// TODO: async load all and wait
		
		
		GoNextSequenceEntry();
	}
	else if (CachedSequenceData->AsyncLoadType == ECAIBStaticIdleSequencerAsyncLoadType::AsyncLoadOnStart)
	{
		CurrentIndex = GetNextSequenceEntryIndex();
		const auto& SequenceEntryData = CachedSequenceData->SequenceEntries[CurrentIndex];

		bool bNeedToLoadFirstAnim = CachedBaseData->bAsyncLoadBeforeStarting || !SequenceEntryData.SoftMontage.IsValid();
		if (CachedBaseData->bAsyncLoadBeforeStarting && !SequenceEntryData.SoftMontage.IsValid())
		{
			SequenceEntryData.SoftMontage.LoadAsync(
				FLoadSoftObjectPathAsyncDelegate::CreateLambda([this] (const FSoftObjectPath&, UObject* LoadedObj)
				{
					FU_ENSURE_VALID(LoadedObj);
					ExecuteCurrentSequenceEntry();
				})
			);
		}
		else
		{
			ExecuteCurrentSequenceEntry();
		}

		// TODO: async load all but dont wait
		// if bNeedToLoadFirstAnim is false dont load current entry as it will already load it
		
	}
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
	
	// race condition if this is called while the async request (if one) wasnt finished
	// this shouldnt happen because an animation is fast to load
	// and perception should not happen until at least a few secs
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

	// this is annoying to async load beforehand because the order can be random
	// we call load sync to be sure we got something to call,
	// but if bAsyncLoadBeforeStarting is true this will not load anything (WIP)
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
	GoBackToStartAcceptanceRadius(50),
	bAsyncLoadBeforeStarting(true)
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
	AsyncLoadType(ECAIBStaticIdleSequencerAsyncLoadType::AsyncLoadOnStart),
	SelectionType(ECAIBStaticIdleBehaviorSequenceType::InOrder),
	bOnResumeGoBackToInitialIndex(false),
	bOnResumeResetAnimationProgress(true)
{}

TSharedPtr<FCAIBStaticIdleBaseRuntimeData> FCAIBStaticIdleBehaviorSequenceData::MakeSharedRuntime() const
{
	return MakeShared<FCAIBStaticIdleSequenceRuntimeData>();
}

FCAIBAIBehaviorStaticIdleFragment::FCAIBAIBehaviorStaticIdleFragment() {}
