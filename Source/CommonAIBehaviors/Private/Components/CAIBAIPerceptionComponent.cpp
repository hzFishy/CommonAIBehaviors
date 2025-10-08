// By hzFishy - 2025 - Do whatever you want with it.


#include "Components/CAIBAIPerceptionComponent.h"
#include "Utility/FUUtilities.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBAIPerceptionComponent::UCAIBAIPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCAIBAIPerceptionComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD
#endif

	OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &ThisClass::TargetPerceptionInfoUpdatedCallback);
	OnTargetPerceptionForgotten.AddUniqueDynamic(this, &ThisClass::TargetPerceptionForgottenCallback);
}

void UCAIBAIPerceptionComponent::OnUnregister()
{
	Super::OnUnregister();

#if WITH_EDITOR
	FU_UTILS_EDITOR_RETURN_NOTGAMEWORLD
#endif

	OnTargetPerceptionInfoUpdated.RemoveDynamic(this, &ThisClass::TargetPerceptionInfoUpdatedCallback);
	OnTargetPerceptionForgotten.RemoveDynamic(this, &ThisClass::TargetPerceptionForgottenCallback);
}

	
	/*----------------------------------------------------------------------------
		Callback
	----------------------------------------------------------------------------*/
void UCAIBAIPerceptionComponent::TargetPerceptionInfoUpdatedCallback(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (UpdateInfo.Target.IsValid())
	{
		
	}
}

void UCAIBAIPerceptionComponent::TargetPerceptionForgottenCallback(AActor* ForgottenActor)
{
	if (IsValid(ForgottenActor))
	{
		
	}
}
