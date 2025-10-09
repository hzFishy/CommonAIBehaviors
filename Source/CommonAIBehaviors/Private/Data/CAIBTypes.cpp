// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBTypes.h"
#include "GameFramework/Character.h"

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
FCAIBBehaviorId::FCAIBBehaviorId():
	Id(0)
{}

FCAIBBehaviorId::FCAIBBehaviorId(FCAIBBehaviorId&& Other):
	Id(Other.Id)
{}

FCAIBBehaviorId::FCAIBBehaviorId(const FCAIBBehaviorId& Other):
	Id(Other.Id)
{}

FCAIBBehaviorId::FCAIBBehaviorId(uint32 InId):
	Id(InId)
{}


FCAIBStateTreeCacheId::FCAIBStateTreeCacheId() {}

FCAIBStateTreeCacheId::FCAIBStateTreeCacheId(const UStateTree* InStateTree, const UScriptStruct* InTaskType):
	StateTree(InStateTree),
	TaskType(InTaskType)
{}


FCAIBBehaviorRuntimeDataBase::FCAIBBehaviorRuntimeDataBase():
	bStarted(false),
	bActive(false)
{}

FCAIBBehaviorRuntimeDataBase::~FCAIBBehaviorRuntimeDataBase() {}

void FCAIBBehaviorRuntimeDataBase::Start()
{
	bStarted = true;
	bActive = true;
}

bool FCAIBBehaviorRuntimeDataBase::CanTick()
{
	return bActive;
}

void FCAIBBehaviorRuntimeDataBase::Tick(float DeltaTime) {}

void FCAIBBehaviorRuntimeDataBase::Stop() {}

void FCAIBBehaviorRuntimeDataBase::Pause()
{
	bActive = false;
}

void FCAIBBehaviorRuntimeDataBase::Resume()
{
	bActive = true;
}

#if CAIB_WITH_DEBUG
FFUMessageBuilder FCAIBBehaviorRuntimeDataBase::GetDebugState() const
{
	return FFUMessageBuilder();
}

void FCAIBBehaviorRuntimeDataBase::DrawDebugState() {}
#endif

void FCAIBBehaviorRuntimeDataBase::SetTargetActor(AActor* InActor)
{
	TargetActor = InActor;

	OnTargetActorSet();
}

void FCAIBBehaviorRuntimeDataBase::OnTargetActorSet() {}

ACharacter* FCAIBBehaviorRuntimeDataBase::GetTargetCharacter() const
{
	return GetTargetActor<ACharacter>();
}

FCAIBAIBehaviorBaseFragment::FCAIBAIBehaviorBaseFragment() {}

FCAIBAIBehaviorDebugMessageEntry::FCAIBAIBehaviorDebugMessageEntry() {}
