// By hzFishy - 2025 - Do whatever you want with it.

#pragma once


#include "NativeGameplayTags.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "CAIBPerceptionTypes.generated.h"


class UCAIBAIPerceptionComponent;
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_SIGHT);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_HEARING);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_TOUCH);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_DAMAGE);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_TEAM);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_PREDICTION);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AI_SENSES_BLUEPRINT);


UINTERFACE(MinimalAPI)
class UCAIBSenseInterface : public UInterface
{
	GENERATED_BODY()
};

class COMMONAIBEHAVIORS_API ICAIBSenseInterface
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetSenseTag() const;

	static FGameplayTag StaticGetSenseTag(const UObject* Object);
};


UCLASS(DisplayName="Common AI Sense Sight")
class COMMONAIBEHAVIORS_API UCAIBAISense_Sight : public UAISense_Sight
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAISense_Sight();
};

UCLASS(DisplayName="Common AI Sense Sight Config")
class COMMONAIBEHAVIORS_API UCAIBAISenseConfig_Sight : public UAISenseConfig_Sight
{
	GENERATED_BODY()

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
public:
	UCAIBAISenseConfig_Sight();

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
	
	// state tree event tag to trigger on first perception
	// state tree event tag to trigger on perception forgotten
};


USTRUCT(BlueprintType, DisplayName="Common AI Stimulus")
struct COMMONAIBEHAVIORS_API FCAIBAIStimulus
{
	GENERATED_BODY()
	
	FCAIBAIStimulus();
	FCAIBAIStimulus(const FAIStimulus& Stimulus);
	FCAIBAIStimulus(const FCAIBAIStimulus& PreviousStim, const FAIStimulus& NewStimulus);

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Strength;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector StimulusLocation;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector ReceiverLocation;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName Tag;

	FAISenseID Type;

	float MaxAge;

	float CurrentAge;

	bool IsExpired() const { return CurrentAge >= MaxAge; }
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	uint32 bSuccessfullySensed:1;

	uint32 bExpired:1;
	
};

USTRUCT(BlueprintType, DisplayName="Common AI Tracked Stimulus Source")
struct COMMONAIBEHAVIORS_API FCAIBTrackedStimulusSource
{
	GENERATED_BODY()
	
	FCAIBTrackedStimulusSource();
	
	FCAIBTrackedStimulusSource(const FActorPerceptionUpdateInfo& UpdateInfo);

	/** Id of to the stimulus source */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 TargetId;

	/** Actor associated to the stimulus (can be null) */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TWeakObjectPtr<AActor> Target;

	/** Updated stimulus */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FCAIBAIStimulus LatestStimulus;
};

USTRUCT()
struct COMMONAIBEHAVIORS_API FCAIBTrackedSensesContainer
{
	GENERATED_BODY()

	FCAIBTrackedSensesContainer();
	
	FCAIBTrackedSensesContainer(UCAIBAIPerceptionComponent* Comp, const FActorPerceptionUpdateInfo& UpdateInfo);

	void AddOrUpdateSense(const FActorPerceptionUpdateInfo& UpdateInfo);

	void RemoveSense(FGameplayTag SenseTag);

	const TMap<FGameplayTag, FCAIBTrackedStimulusSource>& GetMap() { return PerSenseSources; };
	TMap<FGameplayTag, FCAIBTrackedStimulusSource>& GetMutableMap() { return PerSenseSources; };
	
protected:
	TWeakObjectPtr<UCAIBAIPerceptionComponent> PerceptionComponent;
	
	/** Key: sense gameplay tag, see ICAIBSenseInterface */
	TMap<FGameplayTag, FCAIBTrackedStimulusSource> PerSenseSources;
};
