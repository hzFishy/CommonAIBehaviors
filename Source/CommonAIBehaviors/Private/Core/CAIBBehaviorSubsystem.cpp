// By hzFishy - 2025 - Do whatever you want with it.


#include "Core/CAIBBehaviorSubsystem.h"
#include "Asserts/FUAsserts.h"
#include "Console/FUConsole.h"
#include "Core/CAIBCore.h"
#include "Draw/FUDraw.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBBehaviorSubsystem::UCAIBBehaviorSubsystem():
	LatestBehaviorId(0),
	LatestPermanentDebugId(0)
{}

bool UCAIBBehaviorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::PIE || WorldType == EWorldType::Game;
}

void UCAIBBehaviorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCAIBBehaviorSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UCAIBBehaviorSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickActiveBehaviors(DeltaTime);
#if CAIB_WITH_DEBUG
	TickDebugMessages(DeltaTime);
#endif
}

void UCAIBBehaviorSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (auto* PlayerController = InWorld.GetFirstPlayerController())
	{
		PlayerActor = PlayerController->GetPawn().Get();
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

FCAIBBehaviorId UCAIBBehaviorSubsystem::AddBehavior(TSharedPtr<FCAIBBehaviorRuntimeDataBase> Val)
{
	if (!FU_ENSURE_VALID(Val->GetTargetActor())) { return FCAIBBehaviorId(0); }
	
	auto Id = MakeUniqueId();
	ActiveBehaviors.Add(Id, Val);
	return Id;
}

void UCAIBBehaviorSubsystem::StopAndRemoveBehavior(const FCAIBBehaviorId& BehaviorId)
{
	auto BehaviorData = ActiveBehaviors.FindAndRemoveChecked(BehaviorId);

	BehaviorData->Stop();
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

void UCAIBBehaviorSubsystem::TickActiveBehaviors(float DeltaTime)
{
	for (auto It = ActiveBehaviors.CreateIterator(); It; ++It)
	{
		auto& Id = It.Key();
		auto* RuntimeData = It.Value().Get();

		// start if not started yet
		if (!RuntimeData->HasStarted())
		{
			RuntimeData->Start();
		}
		// otherwise, tick
		else
		{
			RuntimeData->Tick(DeltaTime);
		}

#if CAIB_WITH_DEBUG
		if (CAIB::Debug::DebugStatesModeForAllValue)
		{
			const float Distance = FVector::Distance(PlayerActor.Get()->GetActorLocation(), RuntimeData->GetTargetActor()->GetActorLocation());

			if (Distance < CAIB::Debug::DebugStatesMaxDistanceForAllValue)
			{
				const auto& MessageBuilder = RuntimeData->GetDebugState();
				FU::Draw::DrawDebugStringFrame(GWorld->GetWorld(),
					RuntimeData->GetTargetActor()->GetActorLocation(),
					MessageBuilder.GetMessage(),
					FColor::Yellow,
					2
				);
			}
			RuntimeData->DrawDebugState();
		}
#endif
	}
}

#if CAIB_WITH_DEBUG
void UCAIBBehaviorSubsystem::TickDebugMessages(float DeltaTime)
{
	if (CAIB::Debug::DebugStatesModeForAllValue)
	{
		for (auto& Pair : PermanentDebugMessages)
		{
			auto& MessageData = Pair.Value;

			const float Distance = FVector::Distance(PlayerActor.Get()->GetActorLocation(), MessageData.TargetActor->GetActorLocation());

			if (Distance < CAIB::Debug::DebugStatesMaxDistanceForAllValue)
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
#endif
