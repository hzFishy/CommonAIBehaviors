// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBPatrolTypes.h"

#include "AIController.h"
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
}

void FCAIBPatrolBaseRuntimeData::Start()
{
	Super::Start();

	MoveToNextPoint();
}

void FCAIBPatrolBaseRuntimeData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanGoNextPoint())
	{
		MoveToNextPoint();
	}
}

void FCAIBPatrolBaseRuntimeData::Resume()
{
	Super::Resume();

	// TODO: move to latest target point
	// TODO: we need to know if we go back to latest index or if we reset the index to 0
	// TODO: option to reset or not progress time at the point (should be true if we reset to 0)
}

void FCAIBPatrolBaseRuntimeData::Stop()
{
	Super::Stop();

	if (CurrentMoveId.IsValid())
	{
		auto* PathFollowingComponent = GetTargetComponent<UPathFollowingComponent>();
		if (IsValid(PathFollowingComponent) && PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
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
		auto* PathFollowingComponent = Controller->GetComponentByClass<UPathFollowingComponent>();
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
}

void FCAIBPatrolSplineRuntimeData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WaitElapsedTime += DeltaTime;
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBPatrolSplineRuntimeData::GetDebugState() const
{
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
	
	if (SelectedWaitAnim != nullptr)
	{
		MessageBuilder
			.NewLine("CurrentMontage: " + SelectedWaitAnim->AnimMontage->GetName());
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
}

void FCAIBPatrolSplineRuntimeData::OnTargetPointReached()
{
	Super::OnTargetPointReached();

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
			int32 MaxIndex = CurrentPointSplineData.WaitAnimationsData.Num() - 1;
			SelectedWaitAnim = &CurrentPointSplineData.WaitAnimationsData[FMath::RandRange(0,  MaxIndex)];

			// TODO (perf): async load
			auto* LoadedMontage = SelectedWaitAnim->AnimMontage.LoadSynchronous();
			
			const float MontageDuration = TargetCharacterAnimInstance->Montage_Play(
				LoadedMontage,
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
	if (TargetCharacterAnimInstance.IsValid() && SelectedWaitAnim != nullptr)
	{
		TargetCharacterAnimInstance->Montage_Stop(
			0.2,
			SelectedWaitAnim->AnimMontage.Get()
		);
	}
	SelectedWaitAnim = nullptr;
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
	AcceptanceRadius(50)
{}
