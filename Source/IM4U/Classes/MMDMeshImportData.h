// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Factories/FbxAssetImportData.h"
#include "MMDMeshImportData.generated.h"
UENUM()
enum EMMDNormalImportMethod
{
	MMDNIM_ComputeNormals UMETA(DisplayName="Compute Normals"),
	MMDNIM_ImportNormals UMETA(DisplayName="Import Normals"),
	MMDNIM_ImportNormalsAndTangents UMETA(DisplayName="Import Normals and Tangents"),
	MMDNIM_MAX,
};

/**
 * Import data and options used when importing any mesh from MMD
 */
UCLASS(config=EditorUserSettings, configdonotcheckdefaults, abstract)
class UMMDMeshImportData : public UFbxAssetImportData// UMMDAssetImportData
{
	GENERATED_UCLASS_BODY()

	/** Enables importing of mesh LODs from MMD LOD groups, if present in the MMD file */
	UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category=ImportSettings, meta=(OBJRestrict="true", ImportType="Mesh", ToolTip="If enabled, creates LOD models for Unreal meshes from LODs in the import file; If not enabled, only the base mesh from the LOD group is imported"))
	uint32 bImportMeshLODs:1;

	/** Enabling this option will read the tangents(tangent,binormal,normal) from MMD file instead of generating them automatically. */
	UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category=ImportSettings, meta=(ImportType="Mesh"))
	TEnumAsByte<enum EMMDNormalImportMethod> NormalImportMethod;

	bool CanEditChange( const FProperty* InProperty ) const;
};