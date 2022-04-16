// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MMDMeshImportData.h"
#include "IM4UPrivatePCH.h"
#include "CoreMinimal.h"

UMMDMeshImportData::UMMDMeshImportData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NormalImportMethod = MMDNIM_ComputeNormals;
}

bool UMMDMeshImportData::CanEditChange(const FProperty* InProperty) const
{
	bool bMutable = Super::CanEditChange(InProperty);
	UObject* Outer = GetOuter();
	if(Outer && bMutable)
	{
		// Let the parent object handle the editability of our properties
		bMutable = Outer->CanEditChange(InProperty);
	}
	return bMutable;
}