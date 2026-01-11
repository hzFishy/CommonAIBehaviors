// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Data/CAIBAIBehaviorObject.h"
#include "Perception/AISenseConfig_Touch.h"
#include "Perception/AISense_Touch.h"
#include "CAIBTouch.generated.h"


USTRUCT(BlueprintType, DisplayName="AI Sense Touch Proximity Config")
struct FCAIBAISenseTouchProximityConfig
{
	GENERATED_BODY()

	FCAIBAISenseTouchProximityConfig();
	
	/** How many secodns we wait before triggering a touch */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits="seconds"))
	float TriggerTime;
	
	/**
	 * While the entry is active, if the Trigger Actor is at this distance (2D) or lower from the AI Actor
	 * we don't wait anymore and trigger the event.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits="meters"))
	float InstantTriggerDistance;
};


UCLASS(DisplayName="Common AI Sense Touch")
class COMMONAIBEHAVIORS_API UCAIBAISense_Touch : public UAISense_Touch
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAISense_Touch();
};


UCLASS(DisplayName="Common AI Sense Touch Config")
class COMMONAIBEHAVIORS_API UCAIBAISenseConfig_Touch : public UAISenseConfig_Touch
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAISenseConfig_Touch();

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
};


/**
 *  Special behavior object for handling proximity touch events.
 *  You can use one object per AI or one shared one.
 *  One AI Actor can have multiple touch events from various Trigger Actors.
 */
UCLASS(DisplayName="AI Behavior Touch Proximity Object", BlueprintType)
class COMMONAIBEHAVIORS_API UCAIBAIBehaviorTouchProximityObject : public UCAIBAIBehaviorObject
{
	GENERATED_BODY()

protected:
	struct FEntry
	{
		FEntry();
		
		TWeakObjectPtr<AActor> AIActor;
		TWeakObjectPtr<AActor> TriggerActor;
		FCAIBAISenseTouchProximityConfig Config;
		float ElapsedTime;
	};
	
	TArray<FEntry> Entries;;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Common AI Behaviors")
	void AddProximityEntry(AActor* AIActor, AActor* TriggerActor, const FCAIBAISenseTouchProximityConfig& Config);
	
	UFUNCTION(BlueprintCallable, Category="Common AI Behaviors")
	void RemoveProximityEntryWithActors(AActor* AIActor, AActor* TriggerActor);

protected:
	void TriggerProximityTouch(const FEntry& Entry); 
};
