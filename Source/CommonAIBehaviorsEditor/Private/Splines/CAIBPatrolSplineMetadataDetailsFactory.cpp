// By hzFishy - 2025 - Do whatever you want with it.


#include "Splines/CAIBPatrolSplineMetadataDetailsFactory.h"

#include "ComponentVisualizer.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "ISinglePropertyView.h"
#include "Selection.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Editor/ComponentVisualizers/Public/SplineComponentVisualizer.h"
#include "Splines/CAIBPatrolSplineActor.h"
#include "Splines/CAIBPatrolSplineComponent.h"
#include "Splines/CAIBPatrolSplineMetadata.h"
#include "Widgets/Input/SNumericEntryBox.h"


/*
void FTestNotifyHook::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}

void FTestNotifyHook::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                       class FEditPropertyChain* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}

UCAIBPatrolSplineMetadataDetailsFactory::~UCAIBPatrolSplineMetadataDetailsFactory() {}

TSharedPtr<ISplineMetadataDetails> UCAIBPatrolSplineMetadataDetailsFactory::Create()
{
	return MakeShared<FPatrolSplineMetadataDetails>();
}

UClass* UCAIBPatrolSplineMetadataDetailsFactory::GetMetadataClass() const
{
	return UCAIBPatrolSplineMetadata::StaticClass();
}


FPatrolSplineMetadataDetails::~FPatrolSplineMetadataDetails() {}

FName FPatrolSplineMetadataDetails::GetName() const
{
	return FName("PatrolSplineMetadataDetails");
}

FText FPatrolSplineMetadataDetails::GetDisplayName() const
{
	return INVTEXT("Patrol Spline Metadata Details");
}

template<class T>
bool UpdateMultipleValue(TOptional<T>& CurrentValue, T InValue)
{
	if (!CurrentValue.IsSet())
	{
		CurrentValue = InValue;
	}
	else if (CurrentValue.IsSet() && CurrentValue.GetValue() != InValue)
	{
		CurrentValue.Reset();
		return false;
	}

	return true;
}


#define ApplyIfDifferent(VariableName, NewValue, TransacValue) \
{ \
	bool bUpdatedSomething = false; \
	if (auto* Metadata = this->GetMetadata()) \
	{ \
		const FScopedTransaction Transaction(TransacValue); \
		for (int32 Index :  this->CurrentSelectedKeys) \
		{ \
			auto& VarVal = Metadata->SplinePointsData[Index].VariableName; \
			if (VarVal != NewValue) \
			{ \
				VarVal = NewValue; \
				bUpdatedSomething = true; \
			} \
		} \
	} \
	if (bUpdatedSomething) \
	{ \
		 this->OnSetValues(*this); \
	} \
} \

void FPatrolSplineMetadataDetails::Update(USplineComponent* InSplineComponent, const TSet<int32>& InSelectedKeys)
{
	// this is called each frame while at least 1 valid spline point is selected
	
	CurrentSplineComponent = InSplineComponent;
	CurrentSelectedKeys = InSelectedKeys;
	
	MaxWalkSpeed.Reset();

	if (InSplineComponent)
	{
		bool bUpdateTestFloat = true;

		auto* Metadata = Cast<UCAIBPatrolSplineMetadata>(InSplineComponent->GetSplinePointsMetadata());
		if (Metadata)
		{
			for (int32 Index : InSelectedKeys)
			{
				if (Metadata->SplinePointsData.IsValidIndex(Index))
				{
					if (bUpdateTestFloat)
					{
						bUpdateTestFloat = UpdateMultipleValue(MaxWalkSpeed, Metadata->SplinePointsData[Index].MaxWalkSpeed);
					}
				}
			}
		}
	}
}

void FPatrolSplineMetadataDetails::GenerateChildContent(IDetailGroup& DetailGroup)
{
	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	auto* SelectedObject = GEditor->GetSelectedActors()->GetSelectedObject(0);
	auto* SplineActor = Cast<ACAIBPatrolSplineActor>(SelectedObject);
	auto* Metadata = SplineActor->GetMutablePatrolSplineMetadata();

	auto NotifyHook = MakeShared<FTestNotifyHook>();
	auto Params = FSinglePropertyParams();
	Params.NotifyHook = &NotifyHook.Get();
	auto SingleProperty = PropertyModule.CreateSingleProperty(
		Metadata,
		GET_MEMBER_NAME_CHECKED(UCAIBPatrolSplineMetadata, SplinePointsData),
		Params
	);
	TSharedPtr<IPropertyHandle> NewHandle = SingleProperty->GetPropertyHandle();

	TSharedPtr<FComponentVisualizer> Visualizer = GUnrealEd->FindComponentVisualizer(SplineActor->GetPatrolSplineComponent()->GetClass());
	auto SplineVisualizer = StaticCastSharedPtr<FSplineComponentVisualizer>(Visualizer);

	
	uint32 NumberOfChild = 0;  
	if (NewHandle->GetNumChildren(NumberOfChild) == FPropertyAccess::Success)
	{
		int32 DisplayIndex = SplineVisualizer->GetSelectedKeys().Array()[0];
		
		//for (uint32 Index = 0; Index < NumberOfChild; ++Index)  
		{
			TSharedRef<IPropertyHandle> ChildPropertyHandle = NewHandle->GetChildHandle(DisplayIndex).ToSharedRef();  
			uint32 NumberOfChild_Entry = 0;  
			if (ChildPropertyHandle->GetNumChildren(NumberOfChild_Entry) == FPropertyAccess::Success)
			{
				for (uint32 Index_Entry = 0; Index_Entry < NumberOfChild_Entry; ++Index_Entry)  
				{
					TSharedRef<IPropertyHandle> ChildPropertyHandle_Entry = ChildPropertyHandle->GetChildHandle(Index_Entry).ToSharedRef();  
					DetailGroup.AddPropertyRow(ChildPropertyHandle_Entry);
				}
			}
		}
	}
	
	//TSharedPtr<IStructureDataProvider> Struct;
	//auto* Template = NewObject<UPropertyTemplateObject>(GetTransientPackage());
	//SinglePropertyViewTemplate.Reset(Template);
	
	/*for (TFieldIterator<FProperty> PropertyIterator(FCAIBPatrolSplinePointData::StaticStruct()); PropertyIterator; ++PropertyIterator)
	{
		FProperty* Property = *PropertyIterator;

		//auto SingleProperty = PropertyModule.CreateSingleProperty(Metadata->SplinePointsData[1].StaticStruct(), *Property->GetName(), FSinglePropertyParams());
		auto SingleProperty = PropertyModule.CreateSingleProperty(Struct, *Property->GetName(), FSinglePropertyParams());
        
		TSharedPtr<IPropertyHandle> NewHandle = SingleProperty->GetPropertyHandle();
		DetailGroup.AddPropertyRow(NewHandle.ToSharedRef());
	}#1#
	
	/*DetailGroup.AddWidgetRow()
		.Visibility(EVisibility::Visible)
		.NameContent()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(INVTEXT("MaxWalkSpeed"))
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		.ValueContent()
			//.MinDesiredWidth(125.0f)
			//.MaxDesiredWidth(125.0f)
			[
				SNew(SPropertyEditorOptional)
				.Value(this, &FPatrolSplineMetadataDetails::GetMaxWalkSpeed)
				.UndeterminedString(INVTEXT("Multiple"))
				.OnValueCommitted(this, &FPatrolSplineMetadataDetails::OnSetTestFloat)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			];#1#
}

UCAIBPatrolSplineMetadata* FPatrolSplineMetadataDetails::GetMetadata() const
{
	auto* Metadata = CurrentSplineComponent.IsValid() ? Cast<UCAIBPatrolSplineMetadata>(CurrentSplineComponent->GetSplinePointsMetadata()) : nullptr;
	return Metadata;
}

void FPatrolSplineMetadataDetails::OnSetValues(FPatrolSplineMetadataDetails& Details)
{
	Details.CurrentSplineComponent->GetSplinePointsMetadata()->Modify();
	Details.CurrentSplineComponent->UpdateSpline();
	Details.CurrentSplineComponent->bSplineHasBeenEdited = true;
	static FProperty* SplineCurvesProperty = FindFProperty<FProperty>(
		USplineComponent::StaticClass(),
		GET_MEMBER_NAME_CHECKED(USplineComponent, SplineCurves)
	);
	FComponentVisualizer::NotifyPropertyModified(Details.CurrentSplineComponent.Get(), SplineCurvesProperty);
	Details.Update(Details.CurrentSplineComponent.Get(), Details.CurrentSelectedKeys);

	GEditor->RedrawLevelEditingViewports(true);
}

TOptional<TOptional<float>> FPatrolSplineMetadataDetails::GetMaxWalkSpeed() const
{
	return MaxWalkSpeed;
}

void FPatrolSplineMetadataDetails::SetMaxWalkSpeed(float NewValue, ETextCommit::Type CommitInfo)
{
	ApplyIfDifferent(MaxWalkSpeed, NewValue, INVTEXT("Set spline point test float"))
}
*/

/*
TOptional<float> FPatrolSplineMetadataDetails::GetTestFloat() const
{
	return TestFloatValue;
}
void FPatrolSplineMetadataDetails::OnSetTestFloat(float NewValue, ETextCommit::Type CommitInfo)
{
	//ApplyIfDifferent(TestFloat, NewValue, INVTEXT("Set spline point test float"))
}
*/
