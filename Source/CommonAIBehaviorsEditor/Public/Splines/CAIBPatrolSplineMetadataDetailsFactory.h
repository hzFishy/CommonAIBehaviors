// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "SplineMetadataDetailsFactory.h"
//#include "CAIBPatrolSplineMetadataDetailsFactory.generated.h"
class UCAIBPatrolSplineMetadata;


/*
class FTestNotifyHook : public FNotifyHook
{
public:
	virtual ~FTestNotifyHook() = default;
	//virtual void NotifyPreChange( FProperty* PropertyAboutToChange ) override {}
	virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged ) override;
	//virtual void NotifyPreChange( class FEditPropertyChain* PropertyAboutToChange );
	virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, class FEditPropertyChain* PropertyThatChanged ) override;
};


UCLASS()
class COMMONAIBEHAVIORSEDITOR_API UCAIBPatrolSplineMetadataDetailsFactory : public USplineMetadataDetailsFactoryBase
{
	GENERATED_BODY()

	virtual ~UCAIBPatrolSplineMetadataDetailsFactory() override;
	virtual TSharedPtr<ISplineMetadataDetails> Create() override;
	virtual UClass* GetMetadataClass() const override;
};

class COMMONAIBEHAVIORSEDITOR_API FPatrolSplineMetadataDetails : public ISplineMetadataDetails, public TSharedFromThis<FPatrolSplineMetadataDetails>
{
public:

	virtual ~FPatrolSplineMetadataDetails();
	virtual FName GetName() const override;
	virtual FText GetDisplayName() const override;
	virtual void Update(USplineComponent* InSplineComponent, const TSet<int32>& InSelectedKeys) override;
	virtual void GenerateChildContent(IDetailGroup& DetailGroup) override;

	UCAIBPatrolSplineMetadata* GetMetadata() const;
	void OnSetValues(FPatrolSplineMetadataDetails& Details);
	
	TWeakObjectPtr<USplineComponent> CurrentSplineComponent;
	TSet<int32> CurrentSelectedKeys;

	TOptional<TOptional<float>> MaxWalkSpeed;
	TOptional<TOptional<float>> GetMaxWalkSpeed() const;
	void SetMaxWalkSpeed(float NewValue, ETextCommit::Type CommitInfo);
};
*/
