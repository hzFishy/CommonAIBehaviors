// By hzFishy - 2025 - Do whatever you want with it.


#include "Core/CAIBBehaviorSubsystem.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "Asserts/FUAsserts.h"
#include "Console/FUConsole.h"
#include "Core/CAIBCore.h"
#include "Data/CAIBAIBehaviorObject.h"
#include "Draw/FUDraw.h"
#include "Utility/FUOrientedBox.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBBehaviorSubsystem::UCAIBBehaviorSubsystem():
	LatestBehaviorId(0),
	LatestPermanentDebugId(0),
	DebugCurrentTargetIndex(0)
{}

bool UCAIBBehaviorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::PIE || WorldType == EWorldType::Game;
}

void UCAIBBehaviorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnActorAddedToWorldHandle = GetWorld()->AddOnActorSpawnedHandler(
		FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::OnActorAddedToWorld)
	);
	
	OnActorRemovedToWorldHandle = GetWorld()->AddOnActorDestroyedHandler(
		FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::OnActorRemovedToWorld)
	);
}

void UCAIBBehaviorSubsystem::Deinitialize()
{
	Super::Deinitialize();

	GetWorld()->RemoveOnActorSpawnedHandler(OnActorAddedToWorldHandle);
	GetWorld()->RemoveOnActorDestroyedHandler(OnActorRemovedToWorldHandle);
}

void UCAIBBehaviorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickBehaviors(DeltaTime);
	
#if CAIB_WITH_DEBUG
	if (CAIB::Debug::DebugStatesModeValue == 1)
	{
		if (auto* CurrentDebugTarget = GetValid(GetTargetDebugAIPawn()))
		{
			FU::Utils::FFUOrientedBox(CurrentDebugTarget, false).DrawDebugFrame(
				GetWorld(),
				FColor::Yellow,
				2,
				0
			);
		}
	}
	
	TickDebugMessages(DeltaTime);
#endif
}

void UCAIBBehaviorSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	for (auto It = TActorIterator<APawn>(GetWorld()); It; ++It)
	{
		OnActorAddedToWorld(*It);
	}
}

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
FCAIBBehaviorId UCAIBBehaviorSubsystem::MakeUniqueId()
{
	LatestBehaviorId++;
	return FCAIBBehaviorId(LatestBehaviorId);
}

FCAIBBehaviorId UCAIBBehaviorSubsystem::AddActiveBehavior(TSharedPtr<FCAIBBehaviorRuntimeDataBase> Val)
{
	if (!FU_ENSURE_VALID(Val->GetTargetActor())) { return FCAIBBehaviorId(0); }
	
	auto Id = MakeUniqueId();
	RuntimeBehaviors.Add(Id, Val);
	return Id;
}

bool UCAIBBehaviorSubsystem::HasBehavior(const FCAIBBehaviorId& BehaviorId) const
{
	return RuntimeBehaviors.Contains(BehaviorId);
}

void UCAIBBehaviorSubsystem::PauseBehavior(const FCAIBBehaviorId& BehaviorId)
{
	auto& RuntimeBehavior = RuntimeBehaviors[BehaviorId];
	RuntimeBehavior->Pause();
}

void UCAIBBehaviorSubsystem::ResumeBehavior(const FCAIBBehaviorId& BehaviorId)
{
	auto& RuntimeBehavior = RuntimeBehaviors[BehaviorId];
	RuntimeBehavior->Resume();
}

void UCAIBBehaviorSubsystem::StopAndRemoveBehavior(const FCAIBBehaviorId& BehaviorId)
{
	auto RuntimeBehavior = RuntimeBehaviors.FindAndRemoveChecked(BehaviorId);

	RuntimeBehavior->Stop();
}

void UCAIBBehaviorSubsystem::CacheNewBehaviorid(const FCAIBStateTreeCacheId& CacheId, const FCAIBBehaviorId& BehaviorId)
{
	CachedStateTreeBehaviorIds.Emplace(CacheId, BehaviorId);
}

const FCAIBBehaviorId& UCAIBBehaviorSubsystem::GetCachedBehaviorid(const FCAIBStateTreeCacheId& CacheId)
{
	return CachedStateTreeBehaviorIds[CacheId];
}

FCAIBBehaviorId* UCAIBBehaviorSubsystem::GetCachedBehavioridSafe(const FCAIBStateTreeCacheId& CacheId)
{
	if (auto* Id = CachedStateTreeBehaviorIds.Find(CacheId))
	{
		return Id;
	}
	return nullptr;
}

uint32 UCAIBBehaviorSubsystem::AddDebugMessage(const FCAIBAIBehaviorDebugMessageEntry& Entry)
{
	LatestPermanentDebugId++;
	PermanentDebugMessages.Add(LatestPermanentDebugId, Entry);
	return LatestPermanentDebugId;
}

void UCAIBBehaviorSubsystem::RemoveDebugMessage(uint32 Id)
{
	PermanentDebugMessages.Remove(Id);
}

void UCAIBBehaviorSubsystem::AddBehaviorObject(UCAIBAIBehaviorObject* Object)
{
	BehaviorObjects.Add(Object);
}

void UCAIBBehaviorSubsystem::TickBehaviors(float DeltaTime)
{
	for (auto It = RuntimeBehaviors.CreateIterator(); It; ++It)
	{
		auto& Id = It.Key();
		auto* RuntimeData = It.Value().Get();

		// start if not started yet
		if (!RuntimeData->HasStarted())
		{
			RuntimeData->Start();
		}
		// otherwise, tick (by default only active behaviors will tick)
		else if (RuntimeData->CanTick())
		{
			RuntimeData->Tick(DeltaTime);
		}

#if CAIB_WITH_DEBUG
		if (CAIB::Debug::DebugStatesModeValue > 0)
		{
			// check if target mode or all
			bool bCanDraw = CAIB::Debug::DebugStatesModeValue == 2;
				
			if (!bCanDraw && CAIB::Debug::DebugStatesModeValue == 1)
			{
				if (GetValid(RuntimeData->GetTargetActor()) == GetValid(GetTargetDebugAIPawn()))
				{
					bCanDraw = true;
				}
			}
			
			if (bCanDraw && DebugPlayerActor.IsValid() && IsValid(RuntimeData->GetTargetActor()))
			{
				const float Distance = FVector::Distance(DebugPlayerActor.Get()->GetActorLocation(), RuntimeData->GetTargetActor()->GetActorLocation());

				// check distance
				if (CAIB::Debug::DebugStatesMaxDistanceValue <= 0 || Distance < CAIB::Debug::DebugStatesMaxDistanceValue)
				{
					const auto& MessageBuilder = RuntimeData->GetDebugState();
					FU::Draw::DrawDebugStringFrame(GWorld->GetWorld(),
						RuntimeData->GetTargetActor()->GetActorLocation(),
						MessageBuilder.GetMessage(),
						FColor::Yellow,
						2
					);
					
					RuntimeData->DrawDebugState();
				}
			}
		}
#endif
	}

	for (auto It = BehaviorObjects.CreateIterator(); It; ++It)
	{
		auto& BehaviorObject = *It;

		BehaviorObject->Tick(DeltaTime);

		if (BehaviorObject->IsMarkedAsFinished())
		{
			BehaviorObject->Stop();
			It.RemoveCurrent();
		}
	}
}

	
	/*----------------------------------------------------------------------------
		Debug
	----------------------------------------------------------------------------*/
#if CAIB_WITH_DEBUG
void UCAIBBehaviorSubsystem::GoToNextDebugAIPawn()
{
	DebugCurrentTargetIndex++;
	
	if (DebugCurrentTargetIndex >= DebugTargetPawns.Num())
	{
		// reset
		DebugCurrentTargetIndex = 0;
	}
}

void UCAIBBehaviorSubsystem::GoToPreviousDebugAIPawn()
{
	DebugCurrentTargetIndex--;
	
	if (DebugCurrentTargetIndex < 0)
	{
		// reset
		DebugCurrentTargetIndex = DebugTargetPawns.Num()-1;
	}
}

void UCAIBBehaviorSubsystem::TickDebugMessages(float DeltaTime)
{
	if (CAIB::Debug::DebugStatesModeValue > 0)
	{
		for (auto& Pair : PermanentDebugMessages)
		{
			auto& MessageData = Pair.Value;

			// check if target mode or all
			bool bCanDraw = CAIB::Debug::DebugStatesModeValue == 2;
				
			if (!bCanDraw && CAIB::Debug::DebugStatesModeValue == 1)
			{
				if (GetValid(MessageData.TargetActor.Get()) == GetValid(GetTargetDebugAIPawn()))
				{
					bCanDraw = true;
				}
			}
			
			if (bCanDraw)
			{
				const float Distance = FVector::Distance(DebugPlayerActor.Get()->GetActorLocation(), MessageData.TargetActor->GetActorLocation());
				
				if (CAIB::Debug::DebugStatesMaxDistanceValue <= 0 || Distance < CAIB::Debug::DebugStatesMaxDistanceValue)
				{
					FU::Draw::DrawDebugStringFrame(GWorld->GetWorld(),
						MessageData.TargetActor->GetActorLocation(),
						MessageData.MessageBuilder.GetMessage(),
						FColor::Yellow,
						2
					);
				}
			}	
		}
	}
}

void UCAIBBehaviorSubsystem::OnActorAddedToWorld(AActor* Actor)
{
	if (auto* Pawn = Cast<APawn>(Actor))
	{
		if (IsValid(Pawn->GetController()) && Pawn->GetController<AAIController>())
		{
			OnDebugAIPawnAddedToWorld(Pawn);
		}
		else if (!DebugPlayerActor.IsValid() && IsValid(Pawn->GetController()) && Pawn->GetController<APlayerController>())
		{
			DebugPlayerActor = Pawn;
		}
	}
}

void UCAIBBehaviorSubsystem::OnDebugAIPawnAddedToWorld(APawn* InPawn)
{
	// this wont impact DebugCurrentTargetIndex
	DebugTargetPawns.Emplace(InPawn);
}

void UCAIBBehaviorSubsystem::OnActorRemovedToWorld(AActor* Actor)
{
	if (auto* Pawn = Cast<APawn>(Actor))
	{
		if (IsValid(Pawn->GetController()) && Pawn->GetController<AAIController>())
		{
			OnDebugAIPawnRemovedToWorld(Pawn);
		}
	}
}

void UCAIBBehaviorSubsystem::OnDebugAIPawnRemovedToWorld(APawn* InPawn)
{
	// shift DebugCurrentTargetIndex if needed

	int32 IndexToRemove = DebugTargetPawns.Find(MakeWeakObjectPtr(InPawn));

	if (IndexToRemove < DebugCurrentTargetIndex)
	{
		DebugCurrentTargetIndex--;
	}

	if (DebugCurrentTargetIndex >= DebugTargetPawns.Num())
	{
		// reset
		DebugCurrentTargetIndex = 0;
	}
	
	DebugTargetPawns.RemoveAt(IndexToRemove);
}

APawn* UCAIBBehaviorSubsystem::GetTargetDebugAIPawn()
{
	if (DebugTargetPawns.IsValidIndex(DebugCurrentTargetIndex))
	{
		return DebugTargetPawns[DebugCurrentTargetIndex].Get();
	}
	return nullptr;
}
#endif
