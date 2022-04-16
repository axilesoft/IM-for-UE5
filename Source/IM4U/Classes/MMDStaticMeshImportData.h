// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/**
 * Import data and options used when importing a static mesh from MMD
 */

#pragma once
#include "Engine/StaticMesh.h"
#include "MMDMeshImportData.h"
#include "MMDStaticMeshImportData.generated.h"

UENUM()
namespace EVertexColorImportOptionMMD
{
	enum Type
	{
		/** Import the static mesh using the vertex colors from the MMD file */
		Replace,
		/** Ignore vertex colors from the MMD file, and keep the existing mesh vertex colors */
		Ignore,
		/** Override all vertex colors with the specified color */
		Override
	};
}

UCLASS(config=EditorUserSettings, AutoExpandCategories=(Options), MinimalAPI)
class UMMDStaticMeshImportData : public UMMDMeshImportData
{
	GENERATED_UCLASS_BODY()

	/** For static meshes, enabling this option will combine all meshes in the MMD into a single monolithic mesh in Unreal */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category=ImportSettings, meta=(ImportType="StaticMesh"))
	FName StaticMeshLODGroup;

	/** Specify how vertex colors should be imported */
	UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category=ImportSettings, meta=(OBJRestrict="true", ImportType="StaticMesh"))
	TEnumAsByte<EVertexColorImportOptionMMD::Type> VertexColorImportOption;

	/** Specify override color in the case that VertexColorImportOption is set to Override */
	UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category=ImportSettings, meta=(OBJRestrict="true", ImportType="StaticMesh"))
	FColor VertexOverrideColor;

	/** Disabling this option will keep degenerate triangles found.  In general you should leave this option on. */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = ImportSettings, meta = (ImportType = "StaticMesh"))
	uint32 bRemoveDegenerates:1;
	
	UPROPERTY(EditAnywhere, config, AdvancedDisplay, Category=ImportSettings, meta=(ImportType="StaticMesh"))
	uint32 bGenerateLightmapUVs:1;

	/** If checked, one convex hull per UCX_ prefixed collision mesh will be generated instead of decomposing into multiple hulls */
	UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category=ImportSettings, meta=(OBJRestrict="true", ImportType="StaticMesh"))
	uint32 bOneConvexHullPerUCX:1;

	/** If checked, collision will automatically be generated (ignored if custom collision is imported or used). */
	UPROPERTY(EditAnywhere, config, Category = ImportSettings, meta=(OBJRestrict="true", ImportType="StaticMesh"))
	uint32 bAutoGenerateCollision : 1;

	/** Gets or creates MMD import data for the specified static mesh */
	static UMMDStaticMeshImportData* GetImportDataForStaticMesh(UStaticMesh* StaticMesh, UMMDStaticMeshImportData* TemplateForCreation);

	bool CanEditChange( const FProperty* InProperty ) const;
};



