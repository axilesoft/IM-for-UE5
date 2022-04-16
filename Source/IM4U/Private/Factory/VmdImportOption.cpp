
#include "Factory/VmdImportOption.h"
#include "../IM4UPrivatePCH.h"

#if 0
void SVMDImportOptions::Construct(const FArguments& InArgs)
{
	WidgetWindow = InArgs._WidgetWindow;

	// Make array of enum pointers
	TSharedPtr<EVMDImportType> DataTableTypePtr = MakeShareable(new EVMDImportType(EVMD_BoneMotionOnly));
	ImportTypes.Add(DataTableTypePtr);
	/*
	ImportTypes.Add(MakeShareable(new ECSVImportType(ECSV_CurveTable)));
	ImportTypes.Add(MakeShareable(new ECSVImportType(ECSV_CurveFloat)));
	ImportTypes.Add(MakeShareable(new ECSVImportType(ECSV_CurveVector)));

	// Find table row struct info
	RowStructs = FDataTableEditorUtils::GetPossibleStructs();
	*/
	// Create widget
	this->ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush(TEXT("Menu.Background")))
			.Padding(10)
			[
				SNew(SVerticalBox)
				// Import type
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ChooseAssetType", "Import As:"))
				]
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SAssignNew(ImportTypeCombo, SComboBox< TSharedPtr<EVMDImportType> >)
						.OptionsSource(&ImportTypes)
						.OnGenerateWidget(this, &SVMDImportOptions::MakeImportTypeItemWidget)
						[
							SNew(STextBlock)
							.Text(this, &SVMDImportOptions::GetSelectedItemText)
						]
					]
#if 0
				// Data row struct
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ChooseRowType", "Choose DataTable Row Type:"))
						.Visibility(this, &SVMDImportOptions::GetTableRowOptionVis)
					]
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SAssignNew(RowStructCombo, SComboBox<UScriptStruct*>)
						.OptionsSource(&RowStructs)
						.OnGenerateWidget(this, &SCSVImportOptions::MakeRowStructItemWidget)
						.Visibility(this, &SCSVImportOptions::GetTableRowOptionVis)
						[
							SNew(STextBlock)
							.Text(this, &SCSVImportOptions::GetSelectedRowOptionText)
						]
					]
				// Curve interpolation
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ChooseCurveType", "Choose Curve Interpolation Type:"))
						.Visibility(this, &SVMDImportOptions::GetCurveTypeVis)
					]
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SAssignNew(CurveInterpCombo, SComboBox<CurveInterpModePtr>)
						.OptionsSource(&CurveInterpModes)
						.OnGenerateWidget(this, &SVMDImportOptions::MakeCurveTypeWidget)
						.Visibility(this, &SVMDImportOptions::GetCurveTypeVis)
						[
							SNew(STextBlock)
							.Text(this, &SVMDImportOptions::GetSelectedCurveTypeText)
						]
					]
#endif
				// Ok/Cancel
				+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
							.Text(LOCTEXT("OK", "OK"))
							.OnClicked(this, &SVMDImportOptions::OnImport)
						]
						+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SButton)
								.Text(LOCTEXT("Cancel", "Cancel"))
								.OnClicked(this, &SVMDImportOptions::OnCancel)
							]
					]
			]
		];
#if 0
	// set-up selection
	ImportTypeCombo->SetSelectedItem(DataTableTypePtr);

	// Populate the valid interploation modes
	{
		CurveInterpModes.Add(MakeShareable(new ERichCurveInterpMode(ERichCurveInterpMode::RCIM_Constant)));
		CurveInterpModes.Add(MakeShareable(new ERichCurveInterpMode(ERichCurveInterpMode::RCIM_Linear)));
		CurveInterpModes.Add(MakeShareable(new ERichCurveInterpMode(ERichCurveInterpMode::RCIM_Cubic)));
	}
#endif
	// NB: Both combo boxes default to first item in their options lists as initially selected item
}
#if 0
/** If we should import */
bool SVMDImportOptions::ShouldImport()
{
	return  ((SelectedStruct != NULL) || GetSelectedImportType() != ECSV_DataTable) && bImport;
}

/** Get the row struct we selected */
UScriptStruct* SVMDImportOptions::GetSelectedRowStruct()
{
	return SelectedStruct;
}

/** Get the import type we selected */
ECSVImportType SVMDImportOptions::GetSelectedImportType()
{
	return SelectedImportType;
}

/** Get the interpolation mode we selected */
ERichCurveInterpMode SVMDImportOptions::GetSelectedCurveIterpMode()
{
	return SelectedCurveInterpMode;
}

/** Whether to show table row options */
EVisibility SVMDImportOptions::GetTableRowOptionVis() const
{
	return (ImportTypeCombo.IsValid() && *ImportTypeCombo->GetSelectedItem() == ECSV_DataTable) ? EVisibility::Visible : EVisibility::Collapsed;
}

/** Whether to show table row options */
EVisibility SVMDImportOptions::GetCurveTypeVis() const
{
	return (ImportTypeCombo.IsValid() && *ImportTypeCombo->GetSelectedItem() == ECSV_CurveTable) ? EVisibility::Visible : EVisibility::Collapsed;
}
#endif
FString SVMDImportOptions::GetImportTypeText(TSharedPtr<EVMDImportType> Type) const
{
	FString EnumString;
	if (*Type == EVMD_BoneMotionOnly)
	{
		EnumString = TEXT("BoneMotionOnly");
	}
	else if (*Type == EVMD_AddMorphOnly)
	{
		EnumString = TEXT("AddMorphOnly");
	}
	return EnumString;
}

/** Called to create a widget for each struct */
TSharedRef<SWidget> SVMDImportOptions::MakeImportTypeItemWidget(TSharedPtr<EVMDImportType> Type)
{
	return	SNew(STextBlock)
		.Text(FText::FromString(GetImportTypeText(Type)));
}
#if 0
/** Called to create a widget for each struct */
TSharedRef<SWidget> SVMDImportOptions::MakeRowStructItemWidget(UScriptStruct* Struct)
{
	check(Struct != NULL);
	return	SNew(STextBlock)
		.Text(FText::FromString(Struct->GetName()));
}

FString SVMDImportOptions::GetCurveTypeText(CurveInterpModePtr InterpMode) const
{
	FString EnumString;

	switch (*InterpMode)
	{
	case ERichCurveInterpMode::RCIM_Constant:
		EnumString = TEXT("Constant");
		break;

	case ERichCurveInterpMode::RCIM_Linear:
		EnumString = TEXT("Linear");
		break;

	case ERichCurveInterpMode::RCIM_Cubic:
		EnumString = TEXT("Cubic");
		break;
	}
	return EnumString;
}

/** Called to create a widget for each curve interpolation enum */
TSharedRef<SWidget> SVMDImportOptions::MakeCurveTypeWidget(CurveInterpModePtr InterpMode)
{
	FString Label = GetCurveTypeText(InterpMode);
	return SNew(STextBlock).Text(FText::FromString(Label));
}
#endif
/** Called when 'OK' button is pressed */
FReply SVMDImportOptions::OnImport()
{
	SelectedImportType = *ImportTypeCombo->GetSelectedItem();
	/*
	SelectedStruct = RowStructCombo->GetSelectedItem();
	if (CurveInterpCombo->GetSelectedItem().IsValid())
	{
		SelectedCurveInterpMode = *CurveInterpCombo->GetSelectedItem();
	}*/
	bImport = true;
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

/** Called when 'Cancel' button is pressed */
FReply SVMDImportOptions::OnCancel()
{
	bImport = false;
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

FText SVMDImportOptions::GetSelectedItemText() const
{
	TSharedPtr<EVMDImportType> SelectedType = ImportTypeCombo->GetSelectedItem();

	return (SelectedType.IsValid())
		? FText::FromString(GetImportTypeText(SelectedType))
		: FText::GetEmpty();
}
#if 0
FText SVMDImportOptions::GetSelectedRowOptionText() const
{
	UScriptStruct* SelectedScript = RowStructCombo->GetSelectedItem();
	return (SelectedScript)
		? FText::FromString(SelectedScript->GetName())
		: FText::GetEmpty();
}

FText SVMDImportOptions::GetSelectedCurveTypeText() const
{
	CurveInterpModePtr CurveModePtr = CurveInterpCombo->GetSelectedItem();
	return (CurveModePtr.IsValid())
		? FText::FromString(GetCurveTypeText(CurveModePtr))
		: FText::GetEmpty();
}
#endif

#endif