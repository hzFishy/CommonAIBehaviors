// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Data/CAIBTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "CAIBAIBehaviorComponent.generated.h"


/**
 *  This should be owned by an AI Pawn since most of the data is pawn specific.
 */
UCLASS(ClassGroup=(CommonAIBehaviors), DisplayName="Common AI Behavior Component", meta=(BlueprintSpawnableComponent))
class COMMONAIBEHAVIORS_API UCAIBAIBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
protected:
	UPROPERTY(EditAnywhere, DisplayName="AI Behavior Fragments", Category="CommonAIBehaviors", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FCAIBAIBehaviorBaseFragment>> AIBehaviorFragments;

	UPROPERTY(EditAnywhere, Category="CommonAIBehaviors", BlueprintReadOnly)
	FGameplayTag InitialEvent;

#if WITH_EDITORONLY_DATA
	FDelegateHandle SplineRenderUpdateHandle;
#endif
	
	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAIBehaviorComponent();

	virtual void OnRegister() override;

	virtual void OnUnregister() override;
	
	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
public:
	template<std::derived_from<FCAIBAIBehaviorBaseFragment> T>
	const T* GetAIBehaviorFragment() const
	{
		for (auto& Frag : AIBehaviorFragments)
		{
			if (auto* FragPtr = Frag.GetPtr<T>())
			{
				return FragPtr;
			}
		}
		return nullptr;
	}
	
	template<std::derived_from<FCAIBAIBehaviorBaseFragment> T>
	T* GetMutableAIBehaviorFragment()
	{
		for (auto& Frag : AIBehaviorFragments)
		{
			if (auto* FragPtr = Frag.GetMutablePtr<T>())
			{
				return FragPtr;
			}
		}
		return nullptr;
	}
};
