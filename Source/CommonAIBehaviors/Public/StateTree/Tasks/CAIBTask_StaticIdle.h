// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "StateTreeTaskBase.h"
#include "Data/CAIBTypes.h"
#include "CAIBTask_StaticIdle.generated.h"
class UCAIBAIBehaviorComponent;


USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBStaticIdleInstanceData
{
	GENERATED_BODY()
	
	FCAIBStaticIdleInstanceData();
	
	UPROPERTY(EditAnywhere, Category=Context)
	TObjectPtr<ACharacter> Character;
	
	TWeakObjectPtr<UAnimInstance> CharacterAnimInstance;

	/** Shared for the various types of static idle behaviors */
	FCAIBBehaviorId BehaviorId;
};


/**
 * AI doesn't move and can play a set of one or more animations wit hvarious parameters
 */
USTRUCT(DisplayName="Common Static Idle Task", Category="Common AI Behaviors")
struct COMMONAIBEHAVIORS_API FCAIBTask_StaticIdle : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCAIBStaticIdleInstanceData;

	FCAIBTask_StaticIdle();
	
	
	/*----------------------------------------------------------------------------
		Defauls
	----------------------------------------------------------------------------*/
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
