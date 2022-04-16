// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once

#include "Engine.h"
#include "Factories/Factory.h"
#include "PmxMaterialImport.h"
#include "PmxImporter.h"
#include "MyUObjectTestClsFactory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCategoryPMXFactory, Log, All)

UCLASS()
class UMyUObjectTestClsFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	virtual bool DoesSupportClass(UClass* Class) override;
	virtual UClass* ResolveSupportedClass() override;
	virtual UObject* FactoryCreateNew(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
		) override;

	virtual UObject* FactoryCreateBinary(
		UClass * InClass,
		UObject * InParent,
		FName InName,
		EObjectFlags Flags,
		UObject * Context,
		const TCHAR * Type,
		const uint8 *& Buffer,
		const uint8 * BufferEnd,
		FFeedbackContext * Warn
		) override;

	//////////////////////////////////////

	UPmxMaterialImport pmxMaterialImport;
#if 0
	void AssetsCreateMaterial();

	///////////////////////////////////////
	// Copy From  T:\UE4\UnrealEngine-4.6\Engine\Source\Editor\UnrealEd\Private\Fbx\FbxStaticMeshImport.cpp
	//////////////
	UStaticMesh* ImportStaticMeshAsSingle(
		UObject* InParent, 
		//TArray<FbxNode*>& MeshNodeArray, 
		const FName InName,
		EObjectFlags Flags, 
		//UFbxStaticMeshImportData* TemplateImportData,
		UStaticMesh* InStaticMesh,
		int LODIndex
		);
	//////////////////////////////////////
	//////
	///////////////////////////////////////
	bool BuildStaticMeshFromGeometry(
		//FbxMesh* Mesh,
		UStaticMesh* StaticMesh, 
		//TArray<FFbxMaterial>& MeshMaterials,
		int LODIndex,
		TMap<FVector, FColor>* ExistingVertexColorData
		);
#endif

	///////////////////////////////////////
	// Copy From  T:\UE4\UnrealEngine-4.6\Engine\Source\Editor\UnrealEd\Private\StaticMeshEdit.cpp
	//////////////

	UStaticMesh* CreateStaticMeshFromBrush(
		UObject* Outer,
		FName Name,
		MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
		TArray<UMaterialInterface*>& Materials
		//ABrush* Brush,
		//UModel* Model
		);

	inline bool FVerticesEqual(
		FVector& V1, 
		FVector& V2
		);

	void GetBrushMesh(
		//ABrush* Brush,
		//UModel* Model,
		MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
		struct FRawMesh& OutMesh,
		TArray<UMaterialInterface*>& OutMaterials
		);

	UStaticMesh* CreateStaticMesh(
		struct FRawMesh& RawMesh, 
		TArray<UMaterialInterface*>& Materials,
		UObject* InOuter,
		FName InName
		);
	///////////////////////////////////////

	///////////////////////////////////////
	// Copy From  
	// T:\UE4\UnrealEngine-4.6\Engine\Source\Editor\UnrealEd\Private\Editor.cpp
	//////////////
	AActor* ConvertBrushesToStaticMesh(
		UObject * InParent,
		const FString& InStaticMeshPackageName,
		//TArray<ABrush*>& InBrushesToConvert, 
		MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
		const FVector& InPivotLocation,
		TArray<UMaterialInterface*>& Materials
		);
	///////////////////////////////////////

#if 0 /*Skeltal*/
	///////////////////////////////////////
	// Copy From  
	// T:\UE4\UnrealEngine-4.6\Engine\Source\Editor\UnrealEd\
	//////////////
	USkeletalMesh* UnFbx::FFbxImporter::ImportSkeletalMesh(
		UObject* InParent, 
		TArray<FbxNode*>& NodeArray,
		const FName& Name, 
		EObjectFlags Flags,
		UFbxSkeletalMeshImportData* TemplateImportData,
		FString Filename,
		TArray<FbxShape*> *FbxShapeArray, 
		FSkeletalMeshImportData* OutData,
		bool bCreateRenderData
		);
	///////////////////////////////////////
#endif

};

