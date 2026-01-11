// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/Senses/CAIBTouch.h"

#include "Perception/AIPerceptionSystem.h"


FCAIBAISenseTouchProximityConfig::FCAIBAISenseTouchProximityConfig():
	TriggerTime(0),
	InstantTriggerDistance(10)
{}


UCAIBAISense_Touch::UCAIBAISense_Touch()
{
	
}


UCAIBAISenseConfig_Touch::UCAIBAISenseConfig_Touch()
{
	
}

TSubclassOf<UAISense> UCAIBAISenseConfig_Touch::GetSenseImplementation() const
{
	return UCAIBAISense_Touch::StaticClass();
}


UCAIBAIBehaviorTouchProximityObject::FEntry::FEntry():
	ElapsedTime(0)
{}

void UCAIBAIBehaviorTouchProximityObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		auto& Entry = *It;
		Entry.ElapsedTime += DeltaTime;

		float Distance2D = FVector::Dist2D(Entry.AIActor->GetActorLocation(), Entry.TriggerActor->GetActorLocation());
		
		if (Entry.ElapsedTime >= Entry.Config.TriggerTime || Distance2D <= Entry.Config.InstantTriggerDistance)
		{
			TriggerProximityTouch(Entry);
			It.RemoveCurrent();
		}
	}
}

void UCAIBAIBehaviorTouchProximityObject::AddProximityEntry(AActor* AIActor, AActor* TriggerActor, const FCAIBAISenseTouchProximityConfig& Config)
{
	FEntry Entry;
	Entry.AIActor = AIActor;
	Entry.TriggerActor = TriggerActor;
	Entry.Config = Config;

	if (Entry.Config.TriggerTime > 0)
	{
		Entries.Add(Entry);
	}
	else
	{
		TriggerProximityTouch(Entry);
	}
}
	
void UCAIBAIBehaviorTouchProximityObject::RemoveProximityEntryWithActors(AActor* AIActor, AActor* TriggerActor)
{
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		auto& Entry = *It;
		if (Entry.AIActor.Get() == AIActor && Entry.TriggerActor.Get() == TriggerActor)
		{

			It.RemoveCurrent();
			return;
		}
	}
}

void UCAIBAIBehaviorTouchProximityObject::TriggerProximityTouch(const FEntry& Entry)
{
	check(Entry.AIActor.IsValid() && Entry.TriggerActor.IsValid())
	
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(Entry.AIActor->GetWorld());
	if (PerceptionSystem)
	{
		const FAITouchEvent Event(Entry.AIActor.Get(), Entry.TriggerActor.Get(), Entry.TriggerActor->GetActorLocation());
		PerceptionSystem->OnEvent(Event);
	}
}
