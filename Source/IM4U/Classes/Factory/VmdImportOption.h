// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MainFrame.h"
#include "Modules/ModuleManager.h"
//#include "DirectoryWatcherModule.h"
//#include "../../../DataTableEditor/Public/IDataTableEditor.h"
#include "Engine/UserDefinedStruct.h"
//#include "DataTableEditorUtils.h"

//DEFINE_LOG_CATEGORY(LogVMDImportFactory);

#if 0
/** Enum to indicate what to import CSV as */
enum EVMDImportType
{
	/** Import as UDataTable */
	EVMD_BoneMotionOnly,
	/** Import as UCurveTable */
	EVMD_AddMorphOnly,
};

/** UI to pick options when importing data table */
class SVMDImportOptions : public SCompoundWidget
{
private:
	/** Whether we should go ahead with import */
	bool										bImport;

	/** Window that owns us */
	TWeakPtr< SWindow >							WidgetWindow;

	// Import type

	/** List of import types to pick from, drives combo box */
	TArray< TSharedPtr<EVMDImportType> >						ImportTypes;

	/** The combo box */
	TSharedPtr< SComboBox< TSharedPtr<EVMDImportType> > >		ImportTypeCombo;

	/** Indicates what kind of asset we want to make from the CSV file */
	EVMDImportType												SelectedImportType;

#if 0
	// Row type

	/** Array of row struct options */
	TArray< UScriptStruct* >						RowStructs;

	/** The row struct combo box */
	TSharedPtr< SComboBox<UScriptStruct*> >			RowStructCombo;

	/** The selected row struct */
	UScriptStruct*									SelectedStruct;

	/** Typedef for curve enum pointers */
	typedef TSharedPtr<ERichCurveInterpMode>		CurveInterpModePtr;

	/** The curve interpolation combo box */
	TSharedPtr< SComboBox<CurveInterpModePtr> >		CurveInterpCombo;

	/** All available curve interpolation modes */
	TArray< CurveInterpModePtr >					CurveInterpModes;

	/** The selected curve interpolation type */
	ERichCurveInterpMode							SelectedCurveInterpMode;
#endif
public:
	SLATE_BEGIN_ARGS(SVMDImportOptions)
		: _WidgetWindow()
	{}

	SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_END_ARGS()

	SVMDImportOptions()
		: bImport(false)
		, SelectedImportType(EVMD_BoneMotionOnly)
		//, SelectedStruct(NULL)
	{}

	void Construct(const FArguments& InArgs);

	/** If we should import */
	bool ShouldImport();
#if 0
	/** Get the row struct we selected */
	UScriptStruct* GetSelectedRowStruct();
#endif
	/** Get the import type we selected */
	EVMDImportType GetSelectedImportType();
#if 0
	/** Get the interpolation mode we selected */
	ERichCurveInterpMode GetSelectedCurveIterpMode();

	/** Whether to show table row options */
	EVisibility GetTableRowOptionVis() const;

	/** Whether to show table row options */
	EVisibility GetCurveTypeVis() const;
#endif
	FString GetImportTypeText(TSharedPtr<EVMDImportType> Type) const;

	/** Called to create a widget for each struct */
	TSharedRef<SWidget> MakeImportTypeItemWidget(TSharedPtr<EVMDImportType> Type);
#if 0
	/** Called to create a widget for each struct */
	TSharedRef<SWidget> MakeRowStructItemWidget(UScriptStruct* Struct);

	FString GetCurveTypeText(CurveInterpModePtr InterpMode) const;

	/** Called to create a widget for each curve interpolation enum */
	TSharedRef<SWidget> MakeCurveTypeWidget(CurveInterpModePtr InterpMode);
#endif
	/** Called when 'OK' button is pressed */
	FReply OnImport();

	/** Called when 'Cancel' button is pressed */
	FReply OnCancel();

	FText GetSelectedItemText() const;

	FText GetSelectedRowOptionText() const;

	FText GetSelectedCurveTypeText() const;



	/** Skeleton to use for imported asset. When importing a mesh, leaving this as "None" will create a new skeleton. When importing and animation this MUST be specified to import the asset. */
	UPROPERTY(EditAnywhere, Category = AnimationList, meta = (ImportType = "Animation"))
		TArray<int *> TestArrayList;
};
#endif
