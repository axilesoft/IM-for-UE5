// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.



#pragma once

#include "SlateBasics.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "PmxImportUI.h"

class SPmxOptionWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPmxOptionWindow)
		: _ImportUI(NULL)
		, _WidgetWindow()
		, _FullPath()
		, _ForcedImportType()
		, _IsObjFormat(false)
	{}

	SLATE_ARGUMENT(UPmxImportUI*, ImportUI)
	SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
	SLATE_ARGUMENT(FText, FullPath)
	SLATE_ARGUMENT(TOptional<EPMXImportType>, ForcedImportType)
	SLATE_ARGUMENT(bool, IsObjFormat)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	virtual bool SupportsKeyboardFocus() const override { return true; }

	FReply OnImport()
	{
		bShouldImport = true;
		skipModel = false;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}
	FReply OnImportAll()
	{
		bShouldImportAll = true;
		return OnImport();
	}
	FReply OnDisable()
	{
		skipModel = true;
		return OnCancel();
	}

	FReply OnCancel()
	{
		bShouldImport = false;
		bShouldImportAll = false;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			return OnCancel();
		}

		return FReply::Unhandled();
	}

	bool ShouldImport() const
	{
		return bShouldImport;
	}

	bool ShouldImportAll() const
	{
		return bShouldImportAll;
	}

	bool SkipModel() {
		return skipModel;
	}
	SPmxOptionWindow()
		: ImportUI(NULL)
		, bShouldImport(false)
		, bShouldImportAll(false)
	{}

private:

	bool CanImport() const;

private:
	UPmxImportUI*	ImportUI;
	TWeakPtr< SWindow > WidgetWindow;
	TSharedPtr< SButton > ImportButton;
	bool			bShouldImport;
	bool			bShouldImportAll;
	bool			bIsObjFormat;
	bool			skipModel = false;
};
