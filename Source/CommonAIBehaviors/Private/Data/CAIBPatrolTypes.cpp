// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBPatrolTypes.h"

#include "AIController.h"
#include "Asserts/FUAsserts.h"
#include "Draw/FUDraw.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Splines/CAIBPatrolSplineActor.h"
#include "Splines/CAIBPatrolSplineComponent.h"

	
	/*----------------------------------------------------------------------------
		Patrol
	----------------------------------------------------------------------------*/
FCAIBPatrolSplinePointDataWaitAnimation::FCAIBPatrolSplinePointDataWaitAnimation():
	PlayRate(1)
{}

FCAIBPatrolSplinePointData::FCAIBPatrolSplinePointData():
	bOverrideAnimationDurationWithWaitTime(false)
{}


FCAIBPatrolBaseRuntimeData::FCAIBPatrolBaseRuntimeData():
	CachedPatrolFragment(nullptr),
	CurrentTargetLocation(FVector::ZeroVector),
	bTargetPointReached(false),
	bMoving(false)
{}

void FCAIBPatrolBaseRuntimeData::OnTargetActorSet()
{
	Super::OnTargetActorSet();

	Controller = GetTargetCharacter()->GetController<AAIController>();
	PathFollowingComponent = Controller->GetComponentByClass<UPathFollowingComponent>();
}

void FCAIBPatrolBaseRuntimeData::Start()
{
	Super::Start();

	MoveToNextPoint();
}

void FCAIBPatrolBaseRuntimeData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPaused()) { return; }

	if (CanGoNextPoint())
	{
		MoveToNextPoint();
	}
}

void FCAIBPatrolBaseRuntimeData::Stop()
{
	Super::Stop();

	if (CurrentMoveId.IsValid())
	{
		if (PathFollowingComponent.IsValid() && PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
		{
			PathFollowingComponent->AbortMove(
				*GetTargetActor(),
				FPathFollowingResultFlags::UserAbort,
				CurrentMoveId,
				EPathFollowingVelocityMode::Keep
			);
		}
	}
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBPatrolBaseRuntimeData::GetDebugState() const
{
	if (!IsActive()) { return FFUMessageBuilder(); }
	
	return FFUMessageBuilder()
	       .Append("Patrol Data")
	       .NewLine("Target Location: " + FU::Utils::PrintCompactVector(CurrentTargetLocation));
}

void FCAIBPatrolBaseRuntimeData::DrawDebugState()
{
	if (!IsActive()) { return; }
	
	FU::Draw::DrawDebugSphereFrame(
		GWorld,
		CurrentTargetLocation,
		CachedPatrolFragment->AcceptanceRadius,
		FColor::Green
	);
}
#endif

bool FCAIBPatrolBaseRuntimeData::CanGoNextPoint() const
{
	return bTargetPointReached;
}

void FCAIBPatrolBaseRuntimeData::MoveToNextPoint()
{
	CurrentTargetLocation = GetNextMoveToLocation();

	MoveToPoint();
}

void FCAIBPatrolBaseRuntimeData::MoveToPoint()
{
	OnPreStartMove();

	FAIMoveRequest MoveReq(CurrentTargetLocation);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(false);
	//MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
	//MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(CachedPatrolFragment->AcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(false);
	//MoveReq.SetCanStrafe(bCanStrafe);

	auto Result = Controller->MoveTo(MoveReq);
	if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		CurrentMoveId = Result.MoveId;
		bTargetPointReached = false;
		
		if (PathFinishDelegateHandle.IsValid())
		{
			PathFollowingComponent->OnRequestFinished.Remove(PathFinishDelegateHandle);
		}
		PathFinishDelegateHandle = PathFollowingComponent->OnRequestFinished.AddLambda(
		[this] (FAIRequestID RequestId, const FPathFollowingResult& Result)
			{
				OnTargetPointReached();
			}
		);
		
		OnStartedMove();
	}
}

FVector FCAIBPatrolBaseRuntimeData::GetNextMoveToLocation()
{
	unimplemented();
	return FVector();
}

void FCAIBPatrolBaseRuntimeData::OnPreStartMove() {}

void FCAIBPatrolBaseRuntimeData::OnStartedMove()
{
	bMoving = true;
}

void FCAIBPatrolBaseRuntimeData::OnTargetPointReached()
{
	bTargetPointReached = true;
	bMoving = false;
}


FCAIBPatrolSplineRuntimeData::FCAIBPatrolSplineRuntimeData():
	InitialPointIndex(0),
	bTeleportToInitialPoint(false),
	CachedSplineMaxIndex(-1),
	WaitElapsedTime(0),
	WaitTargetTime(0),
	CurrentTargetPointIndex(-1),
	SelectedFutureWaitAnim(nullptr),
	SelectedWaitAnim(nullptr),
	PreviousTargetPointIndex(0)
{}

void FCAIBPatrolSplineRuntimeData::SetSplineActor(ACAIBPatrolSplineActor* InSplineActor)
{
	SplineActor = InSplineActor;
	CachedSplineMaxIndex = SplineActor->GetPatrolSplineComponent()->GetNumberOfSplinePoints() - 1;
}

void FCAIBPatrolSplineRuntimeData::OnTargetActorSet()
{
	Super::OnTargetActorSet();
	
	TargetCharacterAnimInstance = GetTargetCharacter()->GetMesh()->GetAnimInstance();
}

void FCAIBPatrolSplineRuntimeData::Start()
{
	// teleport before
	if (bTeleportToInitialPoint)
	{
		const FVector& InitialPos = SplineActor->GetPatrolSplineComponent()->GetLocationAtSplinePoint(
			InitialPointIndex,
			ESplineCoordinateSpace::World
		);
		GetTargetCharacter()->TeleportTo(
			InitialPos,
			FRotator::ZeroRotator,
			false,
			true
		);

		// the target will go go to InitialPointIndex + 1
		CurrentTargetPointIndex = InitialPointIndex;
	}
	else
	{
		// the target will go to InitialPointIndex;
		CurrentTargetPointIndex = InitialPointIndex - 1;
	}

	Super::Start();
}

void FCAIBPatrolSplineRuntimeData::Pause()
{
	Super::Pause();

	// stop any anims that might be playing
	StopPlayingAnim();

	// abort any pending move
	if (PathFinishDelegateHandle.IsValid())
	{
		PathFollowingComponent->OnRequestFinished.Remove(PathFinishDelegateHandle);
	}
}

void FCAIBPatrolSplineRuntimeData::Resume()
{
	Super::Resume();

	if (CachedPatrolFragment->bOnResumeResetToSpecificIndex)
	{
		PreviousTargetPointIndex = CurrentTargetPointIndex;
		CurrentTargetPointIndex = CachedPatrolFragment->ResumeIndex;
	
		CurrentTargetLocation = SplineActor->GetPatrolSplineComponent()->GetLocationAtSplinePoint(
			CurrentTargetPointIndex,
			ESplineCoordinateSpace::World
		);
	}

	// option to reset or not progress time at the point (should be true if we reset to 0)
	if (CachedPatrolFragment->bOnResumeResetProgressTime)
	{
		WaitElapsedTime = 0;
	}

	// now we move to the target location
	MoveToPoint();
}

void FCAIBPatrolSplineRuntimeData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPaused()) { return; }

	WaitElapsedTime += DeltaTime;
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBPatrolSplineRuntimeData::GetDebugState() const
{
	if (!IsActive()) { return FFUMessageBuilder(); }
	
	if (IsPaused()) { return FFUMessageBuilder("Paused"); }
	
	auto& CurrentPointSplineData = SplinePointsData[CurrentTargetPointIndex];

	auto MessageBuilder = Super::GetDebugState();
	MessageBuilder
		// General stuff
		.NewLine(TEXT("Target Index: %i"), CurrentTargetPointIndex);

	if (CurrentPointSplineData.MaxWalkSpeed.IsSet() && bMoving)
	{
		// Temp modifiers
		MessageBuilder
			.NewLine(FString::Printf(TEXT("Speed override: %s"),
				*FU::Utils::PrintCompactFloat(CurrentPointSplineData.MaxWalkSpeed.GetValue())));
	}
	
	if (SelectedWaitAnim != nullptr && IsValid(LoadedWaitAnim))
	{
		MessageBuilder
			.NewLine("CurrentMontage: " + LoadedWaitAnim->GetName());
	}
	
	if (!bMoving)
	{
		MessageBuilder
			.NewLine(FString::Printf(TEXT("Wait Time: %.1fs/%.1fs"), WaitElapsedTime, WaitTargetTime));
	}

	return MessageBuilder;
}

void FCAIBPatrolSplineRuntimeData::DrawDebugState()
{
	Super::DrawDebugState();

	for (int i = 0; i < CachedSplineMaxIndex + 1; ++i)
	{
		const auto& Location = SplineActor->GetPatrolSplineComponent()->GetLocationAtSplinePoint(
			i, ESplineCoordinateSpace::World);

		FU::Draw::DrawDebugStringFrame(
			GWorld,
			Location + FVector(0, 0, CachedPatrolFragment->AcceptanceRadius),
			FString::Printf(TEXT("Index %i"), i),
			i == CurrentTargetPointIndex ? FColor::Green : FColor::White
		);

		if (i == CurrentTargetPointIndex) { continue; }

		FU::Draw::DrawDebugSphereFrame(
			GWorld,
			Location,
			CachedPatrolFragment->AcceptanceRadius,
			FColor::White
		);
	}
}
#endif

bool FCAIBPatrolSplineRuntimeData::CanGoNextPoint() const
{
	return Super::CanGoNextPoint() && WaitElapsedTime >= WaitTargetTime;
}

FVector FCAIBPatrolSplineRuntimeData::GetNextMoveToLocation()
{
	PreviousTargetPointIndex = CurrentTargetPointIndex;
	CurrentTargetPointIndex++;
	if (CurrentTargetPointIndex > CachedSplineMaxIndex)
	{
		CurrentTargetPointIndex = 0;
	}
	
	return SplineActor->GetPatrolSplineComponent()->GetLocationAtSplinePoint(
		CurrentTargetPointIndex,
		ESplineCoordinateSpace::World
	);
}

void FCAIBPatrolSplineRuntimeData::OnPreStartMove()
{
	Super::OnPreStartMove();
	
	StopPlayingAnim();
}

void FCAIBPatrolSplineRuntimeData::OnStartedMove()
{
	Super::OnStartedMove();

	// apply "pre point" settings
	auto& CurrentPointSplineData = SplinePointsData[CurrentTargetPointIndex];

	if (CurrentPointSplineData.MaxWalkSpeed.IsSet())
	{
		PreviousMaxWalkSpeed = GetTargetCharacter()->GetCharacterMovement()->MaxWalkSpeed;;
		GetTargetCharacter()->GetCharacterMovement()->MaxWalkSpeed = CurrentPointSplineData.MaxWalkSpeed.GetValue();
	}

	// select for the future the wait anim to play (if any)
	if (!CurrentPointSplineData.WaitAnimationsData.IsEmpty())
	{
		int32 MaxIndex = CurrentPointSplineData.WaitAnimationsData.Num() - 1;
		SelectedFutureWaitAnim = &CurrentPointSplineData.WaitAnimationsData[FMath::RandRange(0,  MaxIndex)];

		// can already be loaded
		if (SelectedFutureWaitAnim->AnimMontage.IsValid())
		{
			LoadedFutureWaitAnim = SelectedFutureWaitAnim->AnimMontage.Get();
			FU_ENSURE(LoadedFutureWaitAnim);
		}
		else if (auto* AM = UAssetManager::GetIfInitialized())
		{
			FutureWaitAnimHandle = AM->GetStreamableManager().RequestAsyncLoad(
				SelectedFutureWaitAnim->AnimMontage.ToSoftObjectPath(),
				FStreamableDelegateWithHandle::CreateLambda([this](TSharedPtr<struct FStreamableHandle> Handle)
				{
					if (FU_ENSURE_WEAKVALID(SelectedFutureWaitAnim->AnimMontage))
					{
						LoadedFutureWaitAnim = SelectedFutureWaitAnim->AnimMontage.Get();
						FU_ENSURE(LoadedFutureWaitAnim);
					}
				}),
				50
			);
		}
	}
}

void FCAIBPatrolSplineRuntimeData::OnTargetPointReached()
{
	Super::OnTargetPointReached();

	if (PathFinishDelegateHandle.IsValid())
	{
		PathFollowingComponent->OnRequestFinished.Remove(PathFinishDelegateHandle);
	}
	
	auto& CurrentPointSplineData = SplinePointsData[CurrentTargetPointIndex];

	// remove potential temporary vars
	if (PreviousMaxWalkSpeed.IsSet())
	{
		GetTargetCharacter()->GetCharacterMovement()->MaxWalkSpeed = PreviousMaxWalkSpeed.GetValue();
		PreviousMaxWalkSpeed.Reset();
	}

	// see if we go directly to the next point or if we wait
	if (CurrentPointSplineData.WaitTime.IsSet() || !CurrentPointSplineData.WaitAnimationsData.IsEmpty())
	{
		float SelectedWaitTime = 0;

		if (CurrentPointSplineData.WaitAnimationsData.IsEmpty())
		{
			SelectedWaitTime = CurrentPointSplineData.WaitTime.GetValue();
		}
		else if (TargetCharacterAnimInstance.IsValid())
		{
			SelectedWaitAnim = SelectedFutureWaitAnim;

			// cancel async request if it wasnt finished
			if (FutureWaitAnimHandle.IsValid() && !FutureWaitAnimHandle->HasLoadCompleted())
			{
				FutureWaitAnimHandle->CancelHandle();
			}
			
			// LoadedFutureWaitAnim is async loaded before reaching the point,
			// to avoid a weird "idle" pose we sync load when we reach this point if it wasnt loaded yet
			// so if the async request was completly this will be seemless,
			// if not a small (should be unnoticeable) freeze will occur
			LoadedWaitAnim = IsValid(LoadedFutureWaitAnim.Get())
				? LoadedFutureWaitAnim.Get() : SelectedWaitAnim->AnimMontage.LoadSynchronous();
			
			const float MontageDuration = TargetCharacterAnimInstance->Montage_Play(
				LoadedWaitAnim.Get(),
				SelectedWaitAnim->PlayRate,
				EMontagePlayReturnType::Duration
			);
			
			SelectedWaitTime = CurrentPointSplineData.bOverrideAnimationDurationWithWaitTime
				? CurrentPointSplineData.WaitTime.GetValue() : MontageDuration;
		}

		WaitElapsedTime = 0;
		WaitTargetTime = SelectedWaitTime;
	}
}

void FCAIBPatrolSplineRuntimeData::StopPlayingAnim()
{
	if (TargetCharacterAnimInstance.IsValid() && SelectedWaitAnim != nullptr && IsValid(LoadedWaitAnim.Get()))
	{
		TargetCharacterAnimInstance->Montage_Stop(
			0.2,
			LoadedWaitAnim.Get()
		);
	}
	SelectedWaitAnim = nullptr;
	LoadedWaitAnim = nullptr;
}


FCAIBAIBehaviorPatrolBaseData::FCAIBAIBehaviorPatrolBaseData() {}

FCAIBAIBehaviorPatrolBaseData::~FCAIBAIBehaviorPatrolBaseData() {}

TSharedPtr<FCAIBPatrolBaseRuntimeData> FCAIBAIBehaviorPatrolBaseData::MakeSharedRuntime() const
{
	return MakeShared<FCAIBPatrolBaseRuntimeData>();
}

FCAIBAIBehaviorPatrolSplineData::FCAIBAIBehaviorPatrolSplineData():
	InitialPointIndex(0),
	bTeleportToInitialPoint(true)
{}

#if WITH_EDITORONLY_DATA
void FCAIBAIBehaviorPatrolSplineData::PostSerialize(const FArchive& Ar)
{
	ResfreshStatus();
}

void FCAIBAIBehaviorPatrolSplineData::ResfreshStatus()
{
	StatusMessage = "Okay";

	if (PatrolSplineActor.IsNull())
	{
		StatusMessage = "PatrolSplineActor is null";
		return;
	}

	if (!PatrolSplineActor.IsValid())
	{
		StatusMessage = "PatrolSplineActor is invalid";
		return;
	}

	// check number of points
	if (SplinePointsData.Num() != PatrolSplineActor->GetPatrolSplineComponent()->GetNumberOfSplinePoints())
	{
		StatusMessage = FString::Printf(
			TEXT("Indexes not matching (Points Data: %i, Spline: %i))"),
			SplinePointsData.Num(), PatrolSplineActor->GetPatrolSplineComponent()->GetNumberOfSplinePoints()
		);
		return;
	}
}
#endif

TSharedPtr<FCAIBPatrolBaseRuntimeData> FCAIBAIBehaviorPatrolSplineData::MakeSharedRuntime() const
{
	auto RuntimeData = MakeShared<FCAIBPatrolSplineRuntimeData>();
	RuntimeData->SetSplineActor(PatrolSplineActor.Get());
	RuntimeData->bTeleportToInitialPoint = bTeleportToInitialPoint;
	RuntimeData->InitialPointIndex = InitialPointIndex;
	RuntimeData->SplinePointsData = SplinePointsData;
	return RuntimeData;
}


FCAIBAIBehaviorPatrolFragment::FCAIBAIBehaviorPatrolFragment():
	AcceptanceRadius(50),
	bOnResumeResetToSpecificIndex(true),
	ResumeIndex(0),
	bOnResumeResetProgressTime(true)
{}
