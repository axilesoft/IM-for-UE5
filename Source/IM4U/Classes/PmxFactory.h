// Copyright 2015 BlackMa9. All Rights Reserved.
// copy from UE4 :: FbxFactory class
#pragma once

#define IMUE5_SUPPORT_MODEL 1

#include "Engine.h"
#include "Factories/Factory.h"
//#include "Factories/FbxFactory.h"
#include "CoreMinimal.h"
#include "Runtime/Core/Public/Logging/TokenizedMessage.h"
#include "Factories.h"
#include "BusyCursor.h"
#include "SSkeletonWidget.h"
#include "ImportUtils/SkelImport.h"
#include "PmxImporter.h"
#include "PmxImportUI.h"
#include "PmxMaterialImport.h"
#include "MMDExtendAsset.h"
#include "PmxFactory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMMD4UE4_PMXFactory, Log, All)




/////////////////////////////////////////////////////////////

UCLASS()
class IM4U_API UPmxFactory : public UFactory // public UFbxFactory
{
	GENERATED_UCLASS_BODY()

	virtual bool FactoryCanImport(const FString& Filename) override;

	class UPmxImportUI* ImportUI;
	// Begin UFactory Interface
	virtual void PostInitProperties() override;
	virtual bool DoesSupportClass(UClass * Class) override;
	virtual UClass* ResolveSupportedClass() override;
	virtual UObject* FactoryCreateBinary(
		UClass* InClass, 
		UObject* InParent, 
		FName InName, 
		EObjectFlags Flags, 
		UObject* Context,
		const TCHAR* Type, 
		const uint8*& Buffer, 
		const uint8* BufferEnd, 
		FFeedbackContext* Warn,
		bool& bOutOperationCanceled
		) override;
	// End UFactory Interface

	enum E_LOAD_ASSETS_TYPE_MMD
	{
		E_MMD_TO_UE4_UNKOWN,
		E_MMD_TO_UE4_SKELTON,
		E_MMD_TO_UE4_STATICMESH,
		E_MMD_TO_UE4_ANIMATION
	};
	E_LOAD_ASSETS_TYPE_MMD importAssetTypeMMD;

	//////////////////////////////////////////////////////////////////////
	USkeletalMesh* ImportSkeletalMesh(
		UObject* InParent,
		MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
		const FName& Name,
		EObjectFlags Flags,
		//UFbxSkeletalMeshImportData* TemplateImportData,
		FString Filename,
		//TArray<FbxShape*> *FbxShapeArray,
		FSkeletalMeshImportData* OutData,
		bool bCreateRenderData
		);
	//////////
	bool ImportBone(
		//TArray<FbxNode*>& NodeArray,
		MMD4UE4::PmxMeshInfo *PmxMeshInfo,
		FSkeletalMeshImportData &ImportData,
		//UFbxSkeletalMeshImportData* TemplateData,
		//TArray<FbxNode*> &SortedLinks,
		bool& bOutDiffPose,
		bool bDisableMissingBindPoseWarning,
		bool & bUseTime0AsRefPose
		);
	////////////
	bool FillSkelMeshImporterFromFbx(
		FSkeletalMeshImportData& ImportData,
		MMD4UE4::PmxMeshInfo *& PmxMeshInfo
		//FbxMesh*& Mesh,
		//FbxSkin* Skin,
		//FbxShape* FbxShape,
		//TArray<FbxNode*> &SortedLinks,
		//const TArray<FbxSurfaceMaterial*>& FbxMaterials
		);
	///////////////////////////////
	/** Create a new asset from the package and objectname and class */
	static UObject* CreateAssetOfClass(
		UClass* AssetClass,
		FString ParentPackageName,
		FString ObjectName,
		bool bAllowReplace = false
		);
	///////////////////////////////
	/* Templated function to create an asset with given package and name */
	template< class T>
	static T * CreateAsset(FString ParentPackageName, FString ObjectName, bool bAllowReplace = false)
	{
		return (T*)CreateAssetOfClass(T::StaticClass(), ParentPackageName, ObjectName, bAllowReplace);
	}
	///////////////////////////////

	void ImportMorphTargetsInternal(
		//TArray<FbxNode*>& SkelMeshNodeArray,
		MMD4UE4::PmxMeshInfo & PmxMeshInfo,
		USkeletalMesh* BaseSkelMesh,
		UObject* InParent,
		const FString& InFilename,
		int32 LODIndex, FSkeletalMeshImportData& BaseImportData
		);
	///////////////////////////////
	// Import Morph target
	void ImportFbxMorphTarget(
		//TArray<FbxNode*> &SkelMeshNodeArray,
		MMD4UE4::PmxMeshInfo & PmxMeshInfo,
		USkeletalMesh* BaseSkelMesh,
		UObject* InParent,
		const FString& Filename,
		int32 LODIndex, FSkeletalMeshImportData& ImportData
		);
	////////////////////////////////
	void AddTokenizedErrorMessage(
		TSharedRef<FTokenizedMessage> Error,
		FName FbxErrorName
		);
	//////////////////////
	UMMDExtendAsset * CreateMMDExtendFromMMDModel(
		UObject* InParent,
		USkeletalMesh* SkeletalMesh, // issue #2: fix param use skeleton mesh
		MMD4UE4::PmxMeshInfo * PmxMeshInfo
		);
protected:

	bool bDetectImportTypeOnImport;

	/** true if the import operation was canceled. */
	bool bOperationCanceled;


	//
	UPmxMaterialImport pmxMaterialImportHelper;
};
