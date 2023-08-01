// Copyright 2015 BlackMa9. All Rights Reserved.

//#include "TestAssetPJ.h"
#include "MyUObjectTestClsFactory.h"
#if 0
#include "IM4UPrivatePCH.h"
#include "MyUObjectTestCls.h"

#include "CoreMinimal.h"
#include "ComponentReregisterContext.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Developer/AssetTools/Public/IAssetTools.h"

#include "PackageTools.h"
#include "RawMesh.h"

#include "PmxImporter.h"
#include "PmdImporter.h"


#include "ObjectTools.h"

#include "EncodeHelper.h"

DEFINE_LOG_CATEGORY(LogCategoryPMXFactory)


UMyUObjectTestClsFactory::UMyUObjectTestClsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UMyUObjectTestCls::StaticClass();
	bCreateNew = false;
	////////////
	bEditorImport = true;
	bText = false;
	Formats.Add(TEXT("pmd_st;MMD-PMD test static mesh debug"));
	Formats.Add(TEXT("pmx_st;MMD-PMX"));
}

bool UMyUObjectTestClsFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UMyUObjectTestCls::StaticClass());
}

UClass* UMyUObjectTestClsFactory::ResolveSupportedClass()
{
	return UMyUObjectTestCls::StaticClass();
}

UObject* UMyUObjectTestClsFactory::FactoryCreateNew(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn
	)
{
	return NewObject<UMyUObjectTestCls>(InParent, InClass, InName, Flags);;
}

UObject* UMyUObjectTestClsFactory::FactoryCreateBinary(
	UClass * InClass,
	UObject * InParent,
	FName InName,
	EObjectFlags Flags,
	UObject * Context,
	const TCHAR * Type,
	const uint8 *& Buffer,
	const uint8 * BufferEnd,
	FFeedbackContext * Warn
	)
{
	//bool endianChk = IsLittleEndian();
	
	// FEditorDelegates will be deprecated in the next engine update, in the future need to create a Pointer to UEditorSubsystem
	// and need to include #include "Subsystems/ImportSubsystem.h"

	// UEditorSubsystemPtr::BroadcastAssetPreImport(this, InClass, InParent, InName, Type);
	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Type);

	check(InClass == UMyUObjectTestCls::StaticClass());

	UMyUObjectTestCls* NewMyAsset = NULL;
	
	
	NewMyAsset = NewObject<UMyUObjectTestCls>(InParent, InClass, InName, Flags);

	//PMXEncodeType pmxEncodeType = PMXEncodeType_ERROR;
	pmxMaterialImport.InitializeBaseValue(InParent);

	bool bIsPmxFormat = false;
	if (FString(Type).Equals(TEXT("pmx_st"), ESearchCase::IgnoreCase))
	{
		//Is PMX format 
		bIsPmxFormat = true;
	}
	if (bIsPmxFormat == false)
	{
		//PMD
		MMD4UE4::PmdMeshInfo PmdMeshInfo;
		bool isPMDBinaryLoad = PmdMeshInfo.PMDLoaderBinary(Buffer, BufferEnd);

		MMD4UE4::PmxMeshInfo PmxMeshInfo;
		PmdMeshInfo.ConvertToPmxFormat(&PmxMeshInfo);
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMD Import Header Complete."));
		///////////
		//PMX
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, NewMyAsset);
#if 0
		SPMXHeader BufferPMXHeaderPtr__Impl;
#endif
		int NodeIndex = 0;
		int NodeIndexMax = 0;
		//MMD4UE4::PmxMeshInfo pmxMeshInfoPtr;
		bool isPMXBinaryLoad = PmxMeshInfo.PMXLoaderBinary(Buffer, BufferEnd);
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Header Complete."));
		////////////////////////////////////////////
		NodeIndexMax += PmxMeshInfo.textureList.Num();
		NodeIndexMax += PmxMeshInfo.materialList.Num();
		NodeIndexMax += 1;//static mesh
		//AssetsCreateTextuer(InParent, Flags, Warn, L"nAHN_TargetMarker.png");
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import TEST Complete."));
		///////////////////////////////////////////
		TArray<UTexture*> textureAssetList;
		for (int k = 0; k < PmxMeshInfo.textureList.Num(); ++k)
		{
			pmxMaterialImport.AssetsCreateTextuer(
				//InParent,
				//Flags,
				//Warn,
				FPaths::GetPath(GetCurrentFilename()),
				PmxMeshInfo.textureList[k].TexturePath,
				textureAssetList
				);

			//if (NewObject)
			{
				NodeIndex++;
				FFormatNamedArguments Args;
				Args.Add(TEXT("NodeIndex"), NodeIndex);
				Args.Add(TEXT("ArrayLength"), NodeIndexMax);// SkelMeshArray.Num());
				GWarn->StatusUpdate(NodeIndex, NodeIndexMax, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
			}
		}
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Texture Extecd Complete."));
		////////////////////////////////////////////

		TArray<UMaterialInterface*> OutMaterials;
		TArray<FString> UVSets;
		for (int k = 0; k < PmxMeshInfo.materialList.Num(); ++k)
		{
			pmxMaterialImport.CreateUnrealMaterial(
				PmxMeshInfo.modelNameJP,
				//InParent,
				PmxMeshInfo.materialList[k],
				true,
				false,
				OutMaterials,
				textureAssetList);
			//if (NewObject)
			{
				NodeIndex++;
				FFormatNamedArguments Args;
				Args.Add(TEXT("NodeIndex"), NodeIndex);
				Args.Add(TEXT("ArrayLength"), NodeIndexMax);// SkelMeshArray.Num());
				GWarn->StatusUpdate(NodeIndex, NodeIndexMax, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
			}
		}
		///////////////////////////////////////////
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Material Extecd Complete."));
		///////////////////////////////////////////
		/*
		UStaticMesh* NewStaticMesh = UMyUObjectTestClsFactory::ImportStaticMeshAsSingle(
		InParent,
		//TArray<FbxNode*>& MeshNodeArray,
		InName,
		Flags,
		//UFbxStaticMeshImportData* TemplateImportData,
		NULL,
		0);
		*/
		FString InStaticMeshPackageName
			= "";
			// = ("/Game/SM_");
		InStaticMeshPackageName += PmxMeshInfo.modelNameJP;
		FVector InPivotLocation;
		ConvertBrushesToStaticMesh(
			InParent,
			InStaticMeshPackageName,
			//TArray<ABrush*>& InBrushesToConvert, 
			&PmxMeshInfo,
			InPivotLocation,
			OutMaterials
			);
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Static Mesh Complete."));

		//if (NewObject)
		{
			NodeIndex++;
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeIndex"), NodeIndex);
			Args.Add(TEXT("ArrayLength"), NodeIndexMax);// SkelMeshArray.Num());
			GWarn->StatusUpdate(NodeIndex, NodeIndexMax, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
		}
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import ALL Complete.[%s]"), *(NewMyAsset->MyStruct.ModelName));
		//////////////////////////////////////////////////
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMD Import ALL Complete.[FileName--PMD]"));
	}
	else
	{
		//PMX
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, NewMyAsset);
#if 0
		SPMXHeader BufferPMXHeaderPtr__Impl;
#endif
		int NodeIndex = 0;
		int NodeIndexMax = 0;
		MMD4UE4::PmxMeshInfo pmxMeshInfoPtr;
		bool isPMXBinaryLoad = pmxMeshInfoPtr.PMXLoaderBinary(Buffer, BufferEnd);
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Header Complete."));
		////////////////////////////////////////////
		NodeIndexMax += pmxMeshInfoPtr.textureList.Num();
		NodeIndexMax += pmxMeshInfoPtr.materialList.Num();
		NodeIndexMax += 1;//static mesh
		//AssetsCreateTextuer(InParent, Flags, Warn, L"nAHN_TargetMarker.png");
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import TEST Complete."));
		///////////////////////////////////////////
		TArray<UTexture*> textureAssetList;
		for (int k = 0; k < pmxMeshInfoPtr.textureList.Num(); ++k)
		{
			pmxMaterialImport.AssetsCreateTextuer(
				//InParent,
				//Flags,
				//Warn,
				FPaths::GetPath(GetCurrentFilename()),
				pmxMeshInfoPtr.textureList[k].TexturePath,
				textureAssetList
				);

			//if (NewObject)
			{
				NodeIndex++;
				FFormatNamedArguments Args;
				Args.Add(TEXT("NodeIndex"), NodeIndex);
				Args.Add(TEXT("ArrayLength"), NodeIndexMax);// SkelMeshArray.Num());
				GWarn->StatusUpdate(NodeIndex, NodeIndexMax, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
			}
		}
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Texture Extecd Complete."));
		////////////////////////////////////////////
		
		TArray<UMaterialInterface*> OutMaterials; 
		TArray<FString> UVSets;
		for (int k = 0; k < pmxMeshInfoPtr.materialList.Num(); ++k)
		{
			pmxMaterialImport.CreateUnrealMaterial(
				pmxMeshInfoPtr.modelNameJP,
				//InParent,
				pmxMeshInfoPtr.materialList[k],
				true,
				false,
				OutMaterials,
				textureAssetList);
			//if (NewObject)
			{
				NodeIndex++;
				FFormatNamedArguments Args;
				Args.Add(TEXT("NodeIndex"), NodeIndex);
				Args.Add(TEXT("ArrayLength"), NodeIndexMax);// SkelMeshArray.Num());
				GWarn->StatusUpdate(NodeIndex, NodeIndexMax, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
			}
		}
		///////////////////////////////////////////
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Material Extecd Complete."));
		///////////////////////////////////////////
		/*
		UStaticMesh* NewStaticMesh = UMyUObjectTestClsFactory::ImportStaticMeshAsSingle(
			InParent,
			//TArray<FbxNode*>& MeshNodeArray, 
			InName,
			Flags,
			//UFbxStaticMeshImportData* TemplateImportData,
			NULL,
			0);
		*/
		FString InStaticMeshPackageName
			= "";
			// = ("/Game/SM_");
		InStaticMeshPackageName += pmxMeshInfoPtr.modelNameJP;
		FVector InPivotLocation;
		ConvertBrushesToStaticMesh(
			InParent,
			InStaticMeshPackageName,
			//TArray<ABrush*>& InBrushesToConvert, 
			&pmxMeshInfoPtr,
			InPivotLocation,
			OutMaterials
			);
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import Static Mesh Complete."));

		//if (NewObject)
		{
			NodeIndex++;
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeIndex"), NodeIndex);
			Args.Add(TEXT("ArrayLength"), NodeIndexMax);// SkelMeshArray.Num());
			GWarn->StatusUpdate(NodeIndex, NodeIndexMax, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
		}
		UE_LOG(LogCategoryPMXFactory, Warning, TEXT("PMX Import ALL Complete.[%s]"), *(NewMyAsset->MyStruct.ModelName));
	}
	return NewMyAsset;
}


#if 0
void UMyUObjectTestClsFactory::AssetsCreateMaterial()
{
	//Copy From UnFbx::FFbxImporter::BuildStaticMeshFromGeometry
	//

	/*
	//
	// create materials
	//
	int32 MaterialCount = 0;
	TArray<UMaterialInterface*> Materials;
	if (ImportOptions->bImportMaterials)
	{
		CreateNodeMaterials(Node, Materials, UVSets);
	}
	else if (ImportOptions->bImportTextures)
	{
		ImportTexturesFromNode(Node);
	}

	MaterialCount = Node->GetMaterialCount();
	check(!ImportOptions->bImportMaterials || Materials.Num() == MaterialCount);

	// Used later to offset the material indices on the raw triangle data
	int32 MaterialIndexOffset = MeshMaterials.Num();

	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
	{
		FFbxMaterial* NewMaterial = new(MeshMaterials)FFbxMaterial;
		FbxSurfaceMaterial *FbxMaterial = Node->GetMaterial(MaterialIndex);
		FString MaterialName = ANSI_TO_TCHAR(MakeName(FbxMaterial->GetName()));
		NewMaterial->FbxMaterial = FbxMaterial;
		if (ImportOptions->bImportMaterials)
		{
			NewMaterial->Material = Materials[MaterialIndex];
		}
		else
		{
			UMaterialInterface* UnrealMaterialInterface = FindObject<UMaterialInterface>(NULL, *MaterialName);
			if (UnrealMaterialInterface == NULL)
			{
				UnrealMaterialInterface = UMaterial::GetDefaultMaterial(MD_Surface);
			}
			NewMaterial->Material = UnrealMaterialInterface;
		}
	}
	*/
}


UStaticMesh* UMyUObjectTestClsFactory::ImportStaticMeshAsSingle(
	UObject* InParent,
	//TArray<FbxNode*>& MeshNodeArray, 
	const FName InName,
	EObjectFlags Flags,
	//UFbxStaticMeshImportData* TemplateImportData,
	UStaticMesh* InStaticMesh,
	int LODIndex)
{
	bool bBuildStatus = true;
	struct ExistingStaticMeshData* ExistMeshDataPtr = NULL;

	// Make sure rendering is done - so we are not changing data being used by collision drawing.
	//FlushRenderingCommands();
	/*
	if (MeshNodeArray.Num() == 0)
	{
		return NULL;
	}
	*/
	/*
	// Count the number of verts
	int32 NumVerts = 0;
	int32 MeshIndex;
	for (MeshIndex = 0; MeshIndex < MeshNodeArray.Num(); MeshIndex++)
	{
		FbxNode* Node = MeshNodeArray[MeshIndex];
		FbxMesh* FbxMesh = Node->GetMesh();

		if (FbxMesh)
		{
			NumVerts += FbxMesh->GetControlPointsCount();

			// If not combining meshes, reset the vert count between meshes
			if (!ImportOptions->bCombineToSingle)
			{
				NumVerts = 0;
			}
		}
	}
	*/
	UObject* Parent;
	Parent = InParent;

	FString MeshName = InName.ToString()+L"_StMesh";//ObjectTools::SanitizeObjectName(InName.ToString());

	// warning for missing smoothing group info
	//CheckSmoothingInfo(MeshNodeArray[0]->GetMesh());

	// Parent package to place new meshes
	UPackage* Package = NULL;

	// create empty mesh
	UStaticMesh*	StaticMesh = NULL;

	UStaticMesh* ExistingMesh = NULL;
	UObject* ExistingObject = NULL;

	// A mapping of vertex positions to their color in the existing static mesh
	TMap<FVector, FColor>		ExistingVertexColorData;
	// If we should get vertex colors. Defaults to the checkbox value available to the user in the import property window
	bool bGetVertexColors = false;// ImportOptions->bReplaceVertexColors;
	FString NewPackageName;

	if (InStaticMesh == NULL || LODIndex == 0)
	{
		// Create a package for each mesh
		NewPackageName = FPackageName::GetLongPackagePath(Parent->GetOutermost()->GetName()) + TEXT("/") + MeshName;
		NewPackageName = PackageTools::SanitizePackageName(NewPackageName);
		Package = CreatePackage(NULL, *NewPackageName);

		ExistingMesh = FindObject<UStaticMesh>(Package, *MeshName);
		ExistingObject = FindObject<UObject>(Package, *MeshName);
	}
	/*
	if (ExistingMesh)
	{
		ExistingMesh->GetVertexColorData(ExistingVertexColorData);

		if (0 == ExistingVertexColorData.Num())
		{
			// If there were no vertex colors, automatically take vertex colors from the file.
			bGetVertexColors = true;
		}

		// Free any RHI resources for existing mesh before we re-create in place.
		ExistingMesh->PreEditChange(NULL);
		ExistMeshDataPtr = SaveExistingStaticMeshData(ExistingMesh);
	}
	else if (ExistingObject)
	{
		// Replacing an object.  Here we go!
		// Delete the existing object
		bool bDeleteSucceeded = ObjectTools::DeleteSingleObject(ExistingObject);

		if (bDeleteSucceeded)
		{
			// Force GC so we can cleanly create a new asset (and not do an 'in place' replacement)
			CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

			// Create a package for each mesh
			Package = CreatePackage(NULL, *NewPackageName);
		}
		else
		{
			// failed to delete
			AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("ContentBrowser_CannotDeleteReferenced", "{0} wasn't created.\n\nThe asset is referenced by other content."), FText::FromString(MeshName))), FFbxErrors::Generic_CannotDeleteReferenced);
			return NULL;
		}

		// Vertex colors should be copied always if there is no existing static mesh.
		bGetVertexColors = true;
	}
	else
	{
		// Vertex colors should be copied always if there is no existing static mesh.
		bGetVertexColors = true;
	}
	if (InStaticMesh != NULL && LODIndex > 0)
	{
		StaticMesh = InStaticMesh;
	}
	else
	*/
	{
		StaticMesh = new(Package, FName(*MeshName), Flags | RF_Public) UStaticMesh(FObjectInitializer());
	}
	if (StaticMesh->SourceModels.Num() < LODIndex + 1)
	{
		// Add one LOD 
		new(StaticMesh->SourceModels) FStaticMeshSourceModel();

		if (StaticMesh->SourceModels.Num() < LODIndex + 1)
		{
			LODIndex = StaticMesh->SourceModels.Num() - 1;
		}
	}
	FStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];
	/*
	if (InStaticMesh != NULL && LODIndex > 0 && !SrcModel.RawMeshBulkData->IsEmpty())
	{
		// clear out the old mesh data
		FRawMesh RawMesh;
		SrcModel.RawMeshBulkData->SaveRawMesh(RawMesh);
	}
	*/
	// make sure it has a new lighting guid
	StaticMesh->LightingGuid = FGuid::NewGuid();

	// Set it to use textured lightmaps. Note that Build Lighting will do the error-checking (texcoordindex exists for all LODs, etc).
	StaticMesh->LightMapResolution = 64;
	StaticMesh->LightMapCoordinateIndex = 1;
	/*
	TArray<FFbxMaterial> MeshMaterials;
	for (MeshIndex = 0; MeshIndex < MeshNodeArray.Num(); MeshIndex++)
	{
		FbxNode* Node = MeshNodeArray[MeshIndex];

		if (Node->GetMesh())
		{
			if (!BuildStaticMeshFromGeometry(Node->GetMesh(), StaticMesh, 
				MeshMaterials, LODIndex, bGetVertexColors ? NULL : &ExistingVertexColorData))
			{
				bBuildStatus = false;
				break;
			}
		}
	}
	*/
	//test
	BuildStaticMeshFromGeometry(
		//Node->GetMesh(),
		StaticMesh,
		//MeshMaterials,
		LODIndex,
		bGetVertexColors ? NULL : &ExistingVertexColorData);
	
	if (bBuildStatus)
	{
		UE_LOG(LogCategoryPMXFactory, Log, TEXT("== Initial material list:"));
		
		/*
		for (int32 MaterialIndex = 0; MaterialIndex < MeshMaterials.Num(); ++MaterialIndex)
		{
			UE_LOG(LogFbx, Log, TEXT("%d: %s"), MaterialIndex, *MeshMaterials[MaterialIndex].GetName());
		}

		// Compress the materials array by removing any duplicates.
		bool bDoRemap = false;
		TArray<int32> MaterialMap;
		TArray<FFbxMaterial> UniqueMaterials;
		for (int32 MaterialIndex = 0; MaterialIndex < MeshMaterials.Num(); ++MaterialIndex)
		{
			bool bUnique = true;
			for (int32 OtherMaterialIndex = MaterialIndex - 1; OtherMaterialIndex >= 0; --OtherMaterialIndex)
			{
				if (MeshMaterials[MaterialIndex].FbxMaterial == MeshMaterials[OtherMaterialIndex].FbxMaterial)
				{
					int32 UniqueIndex = MaterialMap[OtherMaterialIndex];

					if (UniqueIndex > MAX_MESH_MATERIAL_INDEX)
					{
						UniqueIndex = MAX_MESH_MATERIAL_INDEX;
					}

					MaterialMap.Add(UniqueIndex);
					bDoRemap = true;
					bUnique = false;
					break;
				}
			}
			if (bUnique)
			{
				int32 UniqueIndex = UniqueMaterials.Add(MeshMaterials[MaterialIndex]);

				if (UniqueIndex > MAX_MESH_MATERIAL_INDEX)
				{
					UniqueIndex = MAX_MESH_MATERIAL_INDEX;
				}

				MaterialMap.Add(UniqueIndex);
			}
			else
			{
				UE_LOG(LogFbx, Log, TEXT("  remap %d -> %d"), MaterialIndex, MaterialMap[MaterialIndex]);
			}
		}

		if (UniqueMaterials.Num() > MAX_MESH_MATERIAL_INDEX)
		{
			AddTokenizedErrorMessage(
				FTokenizedMessage::Create(
				EMessageSeverity::Warning,
				FText::Format(LOCTEXT("Error_TooManyMaterials", "StaticMesh has too many({1}) materials. Clamping materials to {0} which may produce unexpected results. Break apart your mesh into multiple pieces to fix this."),
				FText::AsNumber(MAX_MESH_MATERIAL_INDEX),
				FText::AsNumber(UniqueMaterials.Num())
				)),
				FFbxErrors::StaticMesh_TooManyMaterials);
		}

		// Sort materials based on _SkinXX in the name.
		TArray<uint32> SortedMaterialIndex;
		for (int32 MaterialIndex = 0; MaterialIndex < MeshMaterials.Num(); ++MaterialIndex)
		{
			int32 SkinIndex = 0xffff;
			int32 RemappedIndex = MaterialMap[MaterialIndex];
			if (!SortedMaterialIndex.IsValidIndex(RemappedIndex))
			{
				FString FbxMatName = MeshMaterials[RemappedIndex].GetName();

				int32 Offset = FbxMatName.Find(TEXT("_SKIN"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				if (Offset != INDEX_NONE)
				{
					// Chop off the material name so we are left with the number in _SKINXX
					FString SkinXXNumber = FbxMatName.Right(FbxMatName.Len() - (Offset + 1)).RightChop(4);

					if (SkinXXNumber.IsNumeric())
					{
						SkinIndex = FPlatformString::Atoi(*SkinXXNumber);
						bDoRemap = true;
					}
				}

				SortedMaterialIndex.Add(((uint32)SkinIndex << 16) | ((uint32)RemappedIndex & 0xffff));
			}
		}
		SortedMaterialIndex.Sort();

		UE_LOG(LogFbx, Log, TEXT("== After sorting:"));
		TArray<UMaterialInterface*> SortedMaterials;
		for (int32 SortedIndex = 0; SortedIndex < SortedMaterialIndex.Num(); ++SortedIndex)
		{
			int32 RemappedIndex = SortedMaterialIndex[SortedIndex] & 0xffff;
			SortedMaterials.Add(UniqueMaterials[RemappedIndex].Material);
			UE_LOG(LogFbx, Log, TEXT("%d: %s"), SortedIndex, *UniqueMaterials[RemappedIndex].GetName());
		}
		UE_LOG(LogFbx, Log, TEXT("== Mapping table:"));
		for (int32 MaterialIndex = 0; MaterialIndex < MaterialMap.Num(); ++MaterialIndex)
		{
			for (int32 SortedIndex = 0; SortedIndex < SortedMaterialIndex.Num(); ++SortedIndex)
			{
				int32 RemappedIndex = SortedMaterialIndex[SortedIndex] & 0xffff;
				if (MaterialMap[MaterialIndex] == RemappedIndex)
				{
					UE_LOG(LogFbx, Log, TEXT("  sort %d -> %d"), MaterialIndex, SortedIndex);
					MaterialMap[MaterialIndex] = SortedIndex;
					break;
				}
			}
		}

		// Remap material indices.
		int32 MaxMaterialIndex = 0;
		int32 FirstOpenUVChannel = 1;
		{
			FRawMesh RawMesh;
			SrcModel.RawMeshBulkData->LoadRawMesh(RawMesh);

			if (bDoRemap)
			{
				for (int32 TriIndex = 0; TriIndex < RawMesh.FaceMaterialIndices.Num(); ++TriIndex)
				{
					RawMesh.FaceMaterialIndices[TriIndex] = MaterialMap[RawMesh.FaceMaterialIndices[TriIndex]];
				}
			}

			// Compact material indices so that we won't have any sections with zero triangles.
			RawMesh.CompactMaterialIndices();

			// Also compact the sorted materials array.
			if (RawMesh.MaterialIndexToImportIndex.Num() > 0)
			{
				TArray<UMaterialInterface*> OldSortedMaterials;

				Exchange(OldSortedMaterials, SortedMaterials);
				SortedMaterials.Empty(RawMesh.MaterialIndexToImportIndex.Num());
				for (int32 MaterialIndex = 0; MaterialIndex < RawMesh.MaterialIndexToImportIndex.Num(); ++MaterialIndex)
				{
					UMaterialInterface* Material = NULL;
					int32 ImportIndex = RawMesh.MaterialIndexToImportIndex[MaterialIndex];
					if (OldSortedMaterials.IsValidIndex(ImportIndex))
					{
						Material = OldSortedMaterials[ImportIndex];
					}
					SortedMaterials.Add(Material);
				}
			}

			for (int32 TriIndex = 0; TriIndex < RawMesh.FaceMaterialIndices.Num(); ++TriIndex)
			{
				MaxMaterialIndex = FMath::Max<int32>(MaxMaterialIndex, RawMesh.FaceMaterialIndices[TriIndex]);
			}

			for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; i++)
			{
				if (RawMesh.WedgeTexCoords[i].Num() == 0)
				{
					FirstOpenUVChannel = i;
					break;
				}
			}

			SrcModel.RawMeshBulkData->SaveRawMesh(RawMesh);
		}

		// Setup per-section info and the materials array.
		if (LODIndex == 0)
		{
			StaticMesh->Materials.Empty();
		}

		int32 NumMaterials = FMath::Min(SortedMaterials.Num(), MaxMaterialIndex + 1);
		for (int32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex)
		{
			FMeshSectionInfo Info = StaticMesh->SectionInfoMap.Get(LODIndex, MaterialIndex);
			Info.MaterialIndex = StaticMesh->Materials.Num();
			StaticMesh->SectionInfoMap.Set(LODIndex, MaterialIndex, Info);
			StaticMesh->Materials.Add(SortedMaterials[MaterialIndex]);
		}
		*/

		/*`
		// Setup default LOD settings based on the selected LOD group.
		if (ExistingMesh == NULL && LODIndex == 0)
		{
			ITargetPlatform* CurrentPlatform = GetTargetPlatformManagerRef().GetRunningTargetPlatform();
			check(CurrentPlatform);
			const FStaticMeshLODGroup& LODGroup = CurrentPlatform->GetStaticMeshLODSettings().GetLODGroup(ImportOptions->StaticMeshLODGroup);
			int32 NumLODs = LODGroup.GetDefaultNumLODs();
			while (StaticMesh->SourceModels.Num() < NumLODs)
			{
				new (StaticMesh->SourceModels) FStaticMeshSourceModel();
			}
			for (int32 ModelLODIndex = 0; ModelLODIndex < NumLODs; ++ModelLODIndex)
			{
				StaticMesh->SourceModels[ModelLODIndex].ReductionSettings = LODGroup.GetDefaultSettings(ModelLODIndex);
			}
			StaticMesh->LightMapResolution = LODGroup.GetDefaultLightMapResolution();
		}

		if (ExistMeshDataPtr)
		{
			RestoreExistingMeshData(ExistMeshDataPtr, StaticMesh);
		}
		*/
		/*UFbxStaticMeshImportData* ImportData = UFbxStaticMeshImportData::GetImportDataForStaticMesh(StaticMesh, TemplateImportData);
		ImportData->SourceFilePath = FReimportManager::SanitizeImportFilename(UFactory::CurrentFilename, StaticMesh);
		ImportData->SourceFileTimestamp = IFileManager::Get().GetTimeStamp(*UFactory::CurrentFilename).ToString();
		ImportData->bDirty = false;

		// @todo This overrides restored values currently but we need to be able to import over the existing settings if the user chose to do so.
		SrcModel.BuildSettings.bRemoveDegenerates = ImportOptions->bRemoveDegenerates;
		SrcModel.BuildSettings.bRecomputeNormals = ImportOptions->NormalImportMethod == FBXNIM_ComputeNormals;
		SrcModel.BuildSettings.bRecomputeTangents = ImportOptions->NormalImportMethod != FBXNIM_ImportNormalsAndTangents;

		if (ImportOptions->bGenerateLightmapUVs)
		{
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.DstLightmapIndex = FirstOpenUVChannel;
			StaticMesh->LightMapCoordinateIndex = FirstOpenUVChannel;
		}
		else
		{
			SrcModel.BuildSettings.bGenerateLightmapUVs = false;
		}

		StaticMesh->LODGroup = ImportOptions->StaticMeshLODGroup;
		*/
		StaticMesh->Build(false);
		/*
		// The code to check for bad lightmap UVs doesn't scale well with number of triangles.
		// Skip it here because Lightmass will warn about it during a light build anyway.
		bool bWarnOnBadLightmapUVs = false;
		if (bWarnOnBadLightmapUVs)
		{
			TArray< FString > MissingUVSets;
			TArray< FString > BadUVSets;
			TArray< FString > ValidUVSets;
			UStaticMesh::CheckLightMapUVs(StaticMesh, MissingUVSets, BadUVSets, ValidUVSets);

			// NOTE: We don't care about missing UV sets here, just bad ones!
			if (BadUVSets.Num() > 0)
			{
				AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("Error_UVSetLayoutProblem", "Warning: The light map UV set for static mesh '{0}' appears to have layout problems.  Either the triangle UVs are overlapping one another or the UV are out of bounds (0.0 - 1.0 range.)"), FText::FromString(MeshName))), FFbxErrors::StaticMesh_UVSetLayoutProblem);
			}
		}
		*/
	}
	else
	{
		StaticMesh = NULL;
	}

	if (StaticMesh)
	{
		/*
		//collision generation
		if (StaticMesh->bCustomizedCollision == false && ImportOptions->bAutoGenerateCollision)
		{
			FKAggregateGeom & AggGeom = StaticMesh->BodySetup->AggGeom;
			AggGeom.ConvexElems.Empty(1);	//if no custom collision is setup we just regenerate collision when reimport

			const int32 NumDirs = 18;
			TArray<FVector> Dirs;
			Dirs.AddUninitialized(NumDirs);
			for (int32 DirIdx = 0; DirIdx < NumDirs; ++DirIdx) { Dirs[DirIdx] = KDopDir18[DirIdx]; }
			GenerateKDopAsSimpleCollision(StaticMesh, Dirs);
		}

		//warnings based on geometry
		VerifyGeometry(StaticMesh);
		*/
	}

	return StaticMesh;
}

bool UMyUObjectTestClsFactory::BuildStaticMeshFromGeometry(
	//FbxMesh* Mesh,
	UStaticMesh* StaticMesh,
	//TArray<FFbxMaterial>& MeshMaterials,
	int LODIndex,
	TMap<FVector, FColor>* ExistingVertexColorData) 
{
	/*
	check(StaticMesh->SourceModels.IsValidIndex(LODIndex));

	FbxNode* Node = Mesh->GetNode();
	*/
	
	/*
	FStaticMeshSourceModel* NewFStaticMeshSourceModel = new FStaticMeshSourceModel();
	StaticMesh->SourceModels.Add(*NewFStaticMeshSourceModel);
	*/
	FStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];

	//remove the bad polygons before getting any data from mesh
	/*Mesh->RemoveBadPolygons();

	//Get the base layer of the mesh
	FbxLayer* BaseLayer = Mesh->GetLayer(0);
	if (BaseLayer == NULL)
	{
		AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("Error_NoGeometryInMesh", "There is no geometry information in mesh '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::Generic_Mesh_NoGeometry);
		return false;
	}

	//
	//	store the UVs in arrays for fast access in the later looping of triangles 
	//
	// mapping from UVSets to Fbx LayerElementUV
	// Fbx UVSets may be duplicated, remove the duplicated UVSets in the mapping 
	int32 LayerCount = Mesh->GetLayerCount();
	TArray<FString> UVSets;
	UVSets.Empty();
	if (LayerCount > 0)
	{
		int32 UVLayerIndex;
		for (UVLayerIndex = 0; UVLayerIndex<LayerCount; UVLayerIndex++)
		{
			FbxLayer* lLayer = Mesh->GetLayer(UVLayerIndex);
			int UVSetCount = lLayer->GetUVSetCount();
			if (UVSetCount)
			{
				FbxArray<FbxLayerElementUV const*> EleUVs = lLayer->GetUVSets();
				for (int UVIndex = 0; UVIndex<UVSetCount; UVIndex++)
				{
					FbxLayerElementUV const* ElementUV = EleUVs[UVIndex];
					if (ElementUV)
					{
						const char* UVSetName = ElementUV->GetName();
						FString LocalUVSetName = ANSI_TO_TCHAR(UVSetName);

						UVSets.AddUnique(LocalUVSetName);
					}
				}
			}
		}
	}


	// If the the UV sets are named using the following format (UVChannel_X; where X ranges from 1 to 4)
	// we will re-order them based on these names.  Any UV sets that do not follow this naming convention
	// will be slotted into available spaces.
	if (UVSets.Num() > 0)
	{
		for (int32 ChannelNumIdx = 0; ChannelNumIdx < 4; ChannelNumIdx++)
		{
			FString ChannelName = FString::Printf(TEXT("UVChannel_%d"), ChannelNumIdx + 1);
			int32 SetIdx = UVSets.Find(ChannelName);

			// If the specially formatted UVSet name appears in the list and it is in the wrong spot,
			// we will swap it into the correct spot.
			if (SetIdx != INDEX_NONE && SetIdx != ChannelNumIdx)
			{
				// If we are going to swap to a position that is outside the bounds of the
				// array, then we pad out to that spot with empty data.
				for (int32 ArrSize = UVSets.Num(); ArrSize < ChannelNumIdx + 1; ArrSize++)
				{
					UVSets.Add(FString(TEXT("")));
				}
				//Swap the entry into the appropriate spot.
				UVSets.Swap(SetIdx, ChannelNumIdx);
			}
		}
	}


	// See if any of our UV set entry names match LightMapUV.
	for (int UVSetIdx = 0; UVSetIdx < UVSets.Num(); UVSetIdx++)
	{
		if (UVSets[UVSetIdx] == TEXT("LightMapUV"))
		{
			StaticMesh->LightMapCoordinateIndex = UVSetIdx;
		}
	}

	//
	// create materials
	//
	int32 MaterialCount = 0;
	TArray<UMaterialInterface*> Materials;
	if (ImportOptions->bImportMaterials)
	{
		CreateNodeMaterials(Node, Materials, UVSets);
	}
	else if (ImportOptions->bImportTextures)
	{
		ImportTexturesFromNode(Node);
	}

	MaterialCount = Node->GetMaterialCount();
	check(!ImportOptions->bImportMaterials || Materials.Num() == MaterialCount);

	// Used later to offset the material indices on the raw triangle data
	int32 MaterialIndexOffset = MeshMaterials.Num();

	for (int32 MaterialIndex = 0; MaterialIndex<MaterialCount; MaterialIndex++)
	{
		FFbxMaterial* NewMaterial = new(MeshMaterials)FFbxMaterial;
		FbxSurfaceMaterial *FbxMaterial = Node->GetMaterial(MaterialIndex);
		FString MaterialName = ANSI_TO_TCHAR(MakeName(FbxMaterial->GetName()));
		NewMaterial->FbxMaterial = FbxMaterial;
		if (ImportOptions->bImportMaterials)
		{
			NewMaterial->Material = Materials[MaterialIndex];
		}
		else
		{
			UMaterialInterface* UnrealMaterialInterface = FindObject<UMaterialInterface>(NULL, *MaterialName);
			if (UnrealMaterialInterface == NULL)
			{
				UnrealMaterialInterface = UMaterial::GetDefaultMaterial(MD_Surface);
			}
			NewMaterial->Material = UnrealMaterialInterface;
		}
	}

	if (MaterialCount == 0)
	{
		UMaterial* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
		check(DefaultMaterial);
		FFbxMaterial* NewMaterial = new(MeshMaterials)FFbxMaterial;
		NewMaterial->Material = DefaultMaterial;
		NewMaterial->FbxMaterial = NULL;
		MaterialCount = 1;
	}

	//
	// Convert data format to unreal-compatible
	//

	// Must do this before triangulating the mesh due to an FBX bug in TriangulateMeshAdvance
	int32 LayerSmoothingCount = Mesh->GetLayerCount(FbxLayerElement::eSmoothing);
	for (int32 i = 0; i < LayerSmoothingCount; i++)
	{
		GeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(Mesh, i);
	}

	if (!Mesh->IsTriangleMesh())
	{
		UE_LOG(LogFbx, Warning, TEXT("Triangulating static mesh %s"), ANSI_TO_TCHAR(Node->GetName()));

		const bool bReplace = true;
		FbxNodeAttribute* ConvertedNode = GeometryConverter->Triangulate(Mesh, bReplace);

		if (ConvertedNode != NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			Mesh = ConvertedNode->GetNode()->GetMesh();
		}
		else
		{
			AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("Error_FailedToTriangulate", "Unable to triangulate mesh '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::Generic_Mesh_TriangulationFailed);
			return false; // not clean, missing some dealloc
		}
	}

	// renew the base layer
	BaseLayer = Mesh->GetLayer(0);

	//
	//	get the "material index" layer.  Do this AFTER the triangulation step as that may reorder material indices
	//
	FbxLayerElementMaterial* LayerElementMaterial = BaseLayer->GetMaterials();
	FbxLayerElement::EMappingMode MaterialMappingMode = LayerElementMaterial ?
		LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;
	*/
	/*
	//
	//	store the UVs in arrays for fast access in the later looping of triangles 
	//
	int32 UniqueUVCount = UVSets.Num();
	TArray<FbxLayerElementUV const*> LayerElementUV;
	TArray<FbxLayerElement::EReferenceMode> UVReferenceMode;
	TArray<FbxLayerElement::EMappingMode> UVMappingMode;
	if (UniqueUVCount > 0)
	{
		LayerElementUV.AddZeroed(UniqueUVCount);
		UVReferenceMode.AddZeroed(UniqueUVCount);
		UVMappingMode.AddZeroed(UniqueUVCount);
	}
	LayerCount = Mesh->GetLayerCount();
	for (int32 UVIndex = 0; UVIndex < UniqueUVCount; UVIndex++)
	{
		bool bFoundUV = false;
		LayerElementUV[UVIndex] = NULL;
		for (int32 UVLayerIndex = 0; !bFoundUV &&UVLayerIndex<LayerCount; UVLayerIndex++)
		{
			FbxLayer* lLayer = Mesh->GetLayer(UVLayerIndex);
			int UVSetCount = lLayer->GetUVSetCount();
			if (UVSetCount)
			{
				FbxArray<FbxLayerElementUV const*> EleUVs = lLayer->GetUVSets();
				for (int32 FbxUVIndex = 0; FbxUVIndex<UVSetCount; FbxUVIndex++)
				{
					FbxLayerElementUV const* ElementUV = EleUVs[FbxUVIndex];
					if (ElementUV)
					{
						const char* UVSetName = ElementUV->GetName();
						FString LocalUVSetName = ANSI_TO_TCHAR(UVSetName);
						if (LocalUVSetName == UVSets[UVIndex])
						{
							LayerElementUV[UVIndex] = ElementUV;
							UVReferenceMode[UVIndex] = ElementUV->GetReferenceMode();
							UVMappingMode[UVIndex] = ElementUV->GetMappingMode();
							break;
						}
					}
				}
			}
		}
	}
	UniqueUVCount = FMath::Min<int32>(UniqueUVCount, MAX_MESH_TEXTURE_COORDS);


	//
	// get the smoothing group layer
	//
	bool bSmoothingAvailable = false;

	FbxLayerElementSmoothing const* SmoothingInfo = BaseLayer->GetSmoothing();
	FbxLayerElement::EReferenceMode SmoothingReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode SmoothingMappingMode(FbxLayerElement::eByEdge);
	if (SmoothingInfo)
	{
		if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByEdge)
		{
			if (!GeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(Mesh))
			{
				AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("Error_FailedtoConvertSmoothingGroup", "Unable to fully convert the smoothing groups for mesh '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::Generic_Mesh_ConvertSmoothingGroupFailed);
				bSmoothingAvailable = false;
			}
		}

		if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByPolygon)
		{
			bSmoothingAvailable = true;
		}


		SmoothingReferenceMode = SmoothingInfo->GetReferenceMode();
		SmoothingMappingMode = SmoothingInfo->GetMappingMode();
	}

	//
	// get the first vertex color layer
	//
	FbxLayerElementVertexColor* LayerElementVertexColor = BaseLayer->GetVertexColors();
	FbxLayerElement::EReferenceMode VertexColorReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode VertexColorMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementVertexColor)
	{
		VertexColorReferenceMode = LayerElementVertexColor->GetReferenceMode();
		VertexColorMappingMode = LayerElementVertexColor->GetMappingMode();
	}

	//
	// get the first normal layer
	//
	FbxLayerElementNormal* LayerElementNormal = BaseLayer->GetNormals();
	FbxLayerElementTangent* LayerElementTangent = BaseLayer->GetTangents();
	FbxLayerElementBinormal* LayerElementBinormal = BaseLayer->GetBinormals();

	//whether there is normal, tangent and binormal data in this mesh
	bool bHasNTBInformation = LayerElementNormal && LayerElementTangent && LayerElementBinormal;

	FbxLayerElement::EReferenceMode NormalReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode NormalMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementNormal)
	{
		NormalReferenceMode = LayerElementNormal->GetReferenceMode();
		NormalMappingMode = LayerElementNormal->GetMappingMode();
	}

	FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementTangent)
	{
		TangentReferenceMode = LayerElementTangent->GetReferenceMode();
		TangentMappingMode = LayerElementTangent->GetMappingMode();
	}

	FbxLayerElement::EReferenceMode BinormalReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode BinormalMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementBinormal)
	{
		BinormalReferenceMode = LayerElementBinormal->GetReferenceMode();
		BinormalMappingMode = LayerElementBinormal->GetMappingMode();
	}

	//
	// build collision
	//
	bool bImportedCollision = ImportCollisionModels(StaticMesh, GetNodeNameWithoutNamespace(Node));

	if (false && !bImportedCollision && StaticMesh)	//if didn't import collision automatically generate one
	{
		StaticMesh->CreateBodySetup();

		const int32 NumDirs = 18;
		TArray<FVector> Dirs;
		Dirs.AddUninitialized(NumDirs);
		for (int32 DirIdx = 0; DirIdx < NumDirs; ++DirIdx) { Dirs[DirIdx] = KDopDir18[DirIdx]; }
		GenerateKDopAsSimpleCollision(StaticMesh, Dirs);
	}

	bool bEnableCollision = bImportedCollision || (GBuildStaticMeshCollision && LODIndex == 0 && ImportOptions->bRemoveDegenerates);
	for (int32 SectionIndex = MaterialIndexOffset; SectionIndex<MaterialIndexOffset + MaterialCount; SectionIndex++)
	{
		FMeshSectionInfo Info = StaticMesh->SectionInfoMap.Get(LODIndex, SectionIndex);
		Info.bEnableCollision = bEnableCollision;
		StaticMesh->SectionInfoMap.Set(LODIndex, SectionIndex, Info);
	}

	//
	// build un-mesh triangles
	//

	// Construct the matrices for the conversion from right handed to left handed system
	FbxAMatrix TotalMatrix;
	FbxAMatrix TotalMatrixForNormal;
	TotalMatrix = ComputeTotalMatrix(Node);
	TotalMatrixForNormal = TotalMatrix.Inverse();
	TotalMatrixForNormal = TotalMatrixForNormal.Transpose();
	int32 TriangleCount = Mesh->GetPolygonCount();

	if (TriangleCount == 0)
	{
		AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("Error_NoTrianglesFoundInMesh", "No triangles were found on mesh  '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::StaticMesh_NoTriangles);
		return false;
	}

	int32 VertexCount = Mesh->GetControlPointsCount();
	int32 WedgeCount = TriangleCount * 3;
	bool OddNegativeScale = IsOddNegativeScale(TotalMatrix);

	*/
	int32 TriangleCount = 1;
	int32 WedgeCount = TriangleCount * 3;

	// Load the existing raw mesh.
	FRawMesh RawMesh;
	SrcModel.RawMeshBulkData->LoadRawMesh(RawMesh);

	RawMesh.VertexPositions.Empty();
	//RawMesh.VertexPositions.AddZeroed(4);

	int32 VertexOffset = RawMesh.VertexPositions.Num();
	int32 WedgeOffset = RawMesh.WedgeIndices.Num();
	int32 TriangleOffset = RawMesh.FaceMaterialIndices.Num();

	int32 MaxMaterialIndex = 0;

	// Reserve space for attributes.
	RawMesh.FaceMaterialIndices.AddZeroed(TriangleCount);
	RawMesh.FaceSmoothingMasks.AddZeroed(TriangleCount);
	RawMesh.WedgeIndices.AddZeroed(WedgeCount);

	//if (bHasNTBInformation || RawMesh.WedgeTangentX.Num() > 0 || RawMesh.WedgeTangentY.Num() > 0)
	{
		RawMesh.WedgeTangentX.AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTangentX.Num());
		RawMesh.WedgeTangentY.AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTangentY.Num());
	}

	//if (LayerElementNormal || RawMesh.WedgeTangentZ.Num() > 0)
	{
		RawMesh.WedgeTangentZ.AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTangentZ.Num());
	}

	//if (LayerElementVertexColor || ExistingVertexColorData || RawMesh.WedgeColors.Num())
	{
		int32 NumNewColors = WedgeOffset + WedgeCount - RawMesh.WedgeColors.Num();
		int32 FirstNewColor = RawMesh.WedgeColors.Num();
		RawMesh.WedgeColors.AddUninitialized(NumNewColors);
		for (int32 WedgeIndex = FirstNewColor; WedgeIndex < FirstNewColor + NumNewColors; ++WedgeIndex)
		{
			RawMesh.WedgeColors[WedgeIndex] = FColor::White;
		}
	}
	RawMesh.WedgeTexCoords[0].AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTexCoords[0].Num());
	{
		int32 TriangleIndex = 0;
		FVector FbxPosition(10, 0, 0);
		FVector FinalPosition = FbxPosition;// TotalMatrix.MultT(FbxPosition);
		int32 VertexIndex = RawMesh.VertexPositions.Add(/*Converter.ConvertPos*/(FinalPosition));
		int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + VertexIndex;
		RawMesh.WedgeIndices[WedgeIndex]= VertexIndex;
		RawMesh.WedgeTangentX[WedgeIndex] = FVector(1, 0, 0);
		RawMesh.WedgeTangentY[WedgeIndex] = FVector(0, 1, 0);
		RawMesh.WedgeTangentZ[WedgeIndex] = FVector(0, 0, 1);
	}
	{
		int32 TriangleIndex = 0;
		FVector FbxPosition(-10, 0, 0);
		FVector FinalPosition = FbxPosition;// TotalMatrix.MultT(FbxPosition);
		int32 VertexIndex = RawMesh.VertexPositions.Add(/*Converter.ConvertPos*/(FinalPosition));
		int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + VertexIndex;
		RawMesh.WedgeIndices[WedgeIndex] = VertexIndex;
		RawMesh.WedgeTangentX[WedgeIndex] = FVector(1, 0, 0);
		RawMesh.WedgeTangentY[WedgeIndex] = FVector(0, 1, 0);
		RawMesh.WedgeTangentZ[WedgeIndex] = FVector(0, 0, 1);
	}
	{
		int32 TriangleIndex = 0;
		FVector FbxPosition(0, 0, 0);
		FVector FinalPosition = FbxPosition;// TotalMatrix.MultT(FbxPosition);
		int32 VertexIndex = RawMesh.VertexPositions.Add(/*Converter.ConvertPos*/(FinalPosition));
		int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + VertexIndex;
		RawMesh.WedgeIndices[WedgeIndex] = VertexIndex;
		RawMesh.WedgeTangentX[WedgeIndex] = FVector(1, 0, 0);
		RawMesh.WedgeTangentY[WedgeIndex] = FVector(0, 1, 0);
		RawMesh.WedgeTangentZ[WedgeIndex] = FVector(0, 0, 1);
	}
	/*
	// When importing multiple mesh pieces to the same static mesh.  Ensure each mesh piece has the same number of Uv's
	int32 ExistingUVCount = 0;
	for (int32 ExistingUVIndex = 0; ExistingUVIndex < MAX_MESH_TEXTURE_COORDS; ++ExistingUVIndex)
	{
		if (RawMesh.WedgeTexCoords[ExistingUVIndex].Num() > 0)
		{
			// Mesh already has UVs at this index
			++ExistingUVCount;
		}
		else
		{
			// No more UVs
			break;
		}
	}

	int32 UVCount = FMath::Max(UniqueUVCount, ExistingUVCount);

	// At least one UV set must exist.  
	UVCount = FMath::Max(1, UVCount);

	for (int32 UVLayerIndex = 0; UVLayerIndex<UVCount; UVLayerIndex++)
	{
		RawMesh.WedgeTexCoords[UVLayerIndex].AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTexCoords[UVLayerIndex].Num());
	}

	int32 TriangleIndex;
	TMap<int32, int32> IndexMap;
	for (TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
	{
		int32 DestTriangleIndex = TriangleOffset + TriangleIndex;

		for (int32 CornerIndex = 0; CornerIndex<3; CornerIndex++)
		{
			// If there are odd number negative scale, invert the vertex order for triangles
			int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + (OddNegativeScale ? 2 - CornerIndex : CornerIndex);

			// Store vertex index and position.
			int32 ControlPointIndex = Mesh->GetPolygonVertex(TriangleIndex, CornerIndex);
			int32* ExistingIndex = IndexMap.Find(ControlPointIndex);
			if (ExistingIndex)
			{
				RawMesh.WedgeIndices[WedgeIndex] = *ExistingIndex;
			}
			else
			{
				FbxVector4 FbxPosition = Mesh->GetControlPoints()[ControlPointIndex];
				FbxVector4 FinalPosition = TotalMatrix.MultT(FbxPosition);
				int32 VertexIndex = RawMesh.VertexPositions.Add(Converter.ConvertPos(FinalPosition));
				RawMesh.WedgeIndices[WedgeIndex] = VertexIndex;
				IndexMap.Add(ControlPointIndex, VertexIndex);
			}

			//
			// normals, tangents and binormals
			//
			if (LayerElementNormal)
			{
				int TriangleCornerIndex = TriangleIndex * 3 + CornerIndex;
				//normals may have different reference and mapping mode than tangents and binormals
				int NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ?
				ControlPointIndex : TriangleCornerIndex;
				int NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ?
				NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);

				//tangents and binormals share the same reference, mapping mode and index array
				if (bHasNTBInformation)
				{
					int TangentMapIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ?
					ControlPointIndex : TriangleCornerIndex;
					int TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ?
					TangentMapIndex : LayerElementTangent->GetIndexArray().GetAt(TangentMapIndex);

					FbxVector4 TempValue = LayerElementTangent->GetDirectArray().GetAt(TangentValueIndex);
					TempValue = TotalMatrixForNormal.MultT(TempValue);
					FVector TangentX = Converter.ConvertDir(TempValue);
					RawMesh.WedgeTangentX[WedgeIndex] = TangentX.SafeNormal();

					int BinormalMapIndex = (BinormalMappingMode == FbxLayerElement::eByControlPoint) ?
					ControlPointIndex : TriangleCornerIndex;
					int BinormalValueIndex = (BinormalReferenceMode == FbxLayerElement::eDirect) ?
					BinormalMapIndex : LayerElementBinormal->GetIndexArray().GetAt(BinormalMapIndex);

					TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
					TempValue = TotalMatrixForNormal.MultT(TempValue);
					FVector TangentY = -Converter.ConvertDir(TempValue);
					RawMesh.WedgeTangentY[WedgeIndex] = TangentY.SafeNormal();
				}

				FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
				TempValue = TotalMatrixForNormal.MultT(TempValue);
				FVector TangentZ = Converter.ConvertDir(TempValue);
				RawMesh.WedgeTangentZ[WedgeIndex] = TangentZ.SafeNormal();
			}

			//
			// vertex colors
			//
			if (LayerElementVertexColor && !ExistingVertexColorData)
			{
				int32 VertexColorMappingIndex = (VertexColorMappingMode == FbxLayerElement::eByControlPoint) ?
					Mesh->GetPolygonVertex(TriangleIndex, CornerIndex) : (TriangleIndex * 3 + CornerIndex);

				int32 VectorColorIndex = (VertexColorReferenceMode == FbxLayerElement::eDirect) ?
				VertexColorMappingIndex : LayerElementVertexColor->GetIndexArray().GetAt(VertexColorMappingIndex);

				FbxColor VertexColor = LayerElementVertexColor->GetDirectArray().GetAt(VectorColorIndex);

				RawMesh.WedgeColors[WedgeIndex] = FColor(
					uint8(255.f*VertexColor.mRed),
					uint8(255.f*VertexColor.mGreen),
					uint8(255.f*VertexColor.mBlue),
					uint8(255.f*VertexColor.mAlpha)
					);
			}
			else if (ExistingVertexColorData)
			{
				// try to match this triangles current vertex with one that existed in the previous mesh.
				// This is a find in a tmap which uses a fast hash table lookup.
				FVector Position = RawMesh.VertexPositions[RawMesh.WedgeIndices[WedgeIndex]];
				FColor* PaintedColor = ExistingVertexColorData->Find(Position);
				if (PaintedColor)
				{
					// A matching color for this vertex was found
					RawMesh.WedgeColors[WedgeIndex] = *PaintedColor;
				}
			}
		}

		//
		// smoothing mask
		//
		if (bSmoothingAvailable && SmoothingInfo)
		{
			if (SmoothingMappingMode == FbxLayerElement::eByPolygon)
			{
				int lSmoothingIndex = (SmoothingReferenceMode == FbxLayerElement::eDirect) ? TriangleIndex : SmoothingInfo->GetIndexArray().GetAt(TriangleIndex);
				RawMesh.FaceSmoothingMasks[DestTriangleIndex] = SmoothingInfo->GetDirectArray().GetAt(lSmoothingIndex);
			}
			else
			{
				AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("Error_UnsupportedSmoothingGroup", "Unsupported Smoothing group mapping mode on mesh  '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::Generic_Mesh_UnsupportingSmoothingGroup);
			}
		}

		//
		// uvs
		//
		// In FBX file, the same UV may be saved multiple times, i.e., there may be same UV in LayerElementUV
		// So we don't import the duplicate UVs
		int32 UVLayerIndex;
		for (UVLayerIndex = 0; UVLayerIndex<UniqueUVCount; UVLayerIndex++)
		{
			if (LayerElementUV[UVLayerIndex] != NULL)
			{
				for (int32 CornerIndex = 0; CornerIndex<3; CornerIndex++)
				{
					// If there are odd number negative scale, invert the vertex order for triangles
					int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + (OddNegativeScale ? 2 - CornerIndex : CornerIndex);

					int lControlPointIndex = Mesh->GetPolygonVertex(TriangleIndex, CornerIndex);
					int UVMapIndex = (UVMappingMode[UVLayerIndex] == FbxLayerElement::eByControlPoint) ? lControlPointIndex : TriangleIndex * 3 + CornerIndex;
					int32 UVIndex = (UVReferenceMode[UVLayerIndex] == FbxLayerElement::eDirect) ?
					UVMapIndex : LayerElementUV[UVLayerIndex]->GetIndexArray().GetAt(UVMapIndex);
					FbxVector2	UVVector = LayerElementUV[UVLayerIndex]->GetDirectArray().GetAt(UVIndex);

					RawMesh.WedgeTexCoords[UVLayerIndex][WedgeIndex].X = static_cast<float>(UVVector[0]);
					RawMesh.WedgeTexCoords[UVLayerIndex][WedgeIndex].Y = 1.f - static_cast<float>(UVVector[1]);   //flip the Y of UVs for DirectX
				}
			}
		}

		//
		// material index
		//
		int32 MaterialIndex = 0;
		if (MaterialCount>0)
		{
			if (LayerElementMaterial)
			{
				switch (MaterialMappingMode)
				{
					// material index is stored in the IndexArray, not the DirectArray (which is irrelevant with 2009.1)
				case FbxLayerElement::eAllSame:
				{
					MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(0);
				}
				break;
				case FbxLayerElement::eByPolygon:
				{
					MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(TriangleIndex);
				}
				break;
				}
			}
		}
		MaterialIndex += MaterialIndexOffset;

		if (MaterialIndex >= MaterialCount + MaterialIndexOffset || MaterialIndex < 0)
		{
			AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, LOCTEXT("Error_MaterialIndexInconsistency", "Face material index inconsistency - forcing to 0")), FFbxErrors::Generic_Mesh_MaterialIndexInconsistency);
			MaterialIndex = 0;
		}

		RawMesh.FaceMaterialIndices[DestTriangleIndex] = MaterialIndex;
	}


	*/
	// Store the new raw mesh.
	SrcModel.RawMeshBulkData->SaveRawMesh(RawMesh);
	/*
	//
	// clean up.  This needs to happen before the mesh is destroyed
	//
	LayerElementUV.Empty();
	UVReferenceMode.Empty();
	UVMappingMode.Empty();
	*/
	return true;
}

#endif 
///////////////////////////////////////////////////////////////////////////////////////////
//
//	CreateStaticMeshFromBrush - Creates a static mesh from the triangles in a model.
//

UStaticMesh* UMyUObjectTestClsFactory::CreateStaticMeshFromBrush(
	UObject* Outer,
	FName Name,
	MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
	TArray<UMaterialInterface*>& Materials
	//ABrush* Brush,
	//UModel* Model
	)
{
	//GWarn->BeginSlowTask(NSLOCTEXT("UnrealEd", "CreatingStaticMeshE", "Creating static mesh..."), true);

	FRawMesh RawMesh;
	
	GetBrushMesh(/*Brush, Model,*/pmxMeshInfoPtr, RawMesh, Materials);

	UStaticMesh* StaticMesh = CreateStaticMesh(RawMesh, Materials, Outer, Name);
	//GWarn->EndSlowTask();

	return StaticMesh;

}

//
//	FVerticesEqual
//

inline bool UMyUObjectTestClsFactory::FVerticesEqual(
	FVector& V1,
	FVector& V2
	)
{
	if (FMath::Abs(V1.X - V2.X) > THRESH_POINTS_ARE_SAME * 4.0f)
	{
		return 0;
	}

	if (FMath::Abs(V1.Y - V2.Y) > THRESH_POINTS_ARE_SAME * 4.0f)
	{
		return 0;
	}

	if (FMath::Abs(V1.Z - V2.Z) > THRESH_POINTS_ARE_SAME * 4.0f)
	{
		return 0;
	}

	return 1;
}

void UMyUObjectTestClsFactory::GetBrushMesh(
	//ABrush* Brush,
	//UModel* Model,
	MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
	struct FRawMesh& OutMesh,
	TArray<UMaterialInterface*>& OutMaterials
	)
{
	TArray<FPoly> Polys;
	// Calculate the local to world transform for the source brush.

	FMatrix	ActorToWorld = /*Brush ? Brush->ActorToWorld().ToMatrixWithScale() : */FMatrix::Identity;
	bool	ReverseVertices = true;//For MMD
	FVector4	PostSub = /*Brush ? FVector4(Brush->GetActorLocation()) :*/ FVector4(0, 0, 0, 0);


#if 0
	int32 NumPolys = Polys.Num();

	// For each polygon in the model...
	TArray<FVector> TempPositions;
	for (int32 PolygonIndex = 0; PolygonIndex < NumPolys; ++PolygonIndex)
	{
		FPoly& Polygon = Polys[PolygonIndex];

		UMaterialInterface*	Material = Polygon.Material;

		// Find a material index for this polygon.

		int32 MaterialIndex = OutMaterials.AddUnique(Material);

		// Cache the texture coordinate system for this polygon.

		FVector	TextureBase = Polygon.Base - (/*Brush ? Brush->GetPrePivot() : */FVector::ZeroVector),
			TextureX = Polygon.TextureU / UModel::GetGlobalBSPTexelScale(),
			TextureY = Polygon.TextureV / UModel::GetGlobalBSPTexelScale();

		// For each vertex after the first two vertices...
		for (int32 VertexIndex = 2; VertexIndex < Polygon.Vertices.Num(); VertexIndex++)
		{
			// Create a triangle for the vertex.
			OutMesh.FaceMaterialIndices.Add(MaterialIndex);

			// Generate different smoothing mask for each poly to give the mesh hard edges.  Note: only 32 smoothing masks supported.
			OutMesh.FaceSmoothingMasks.Add(1 << (PolygonIndex % 32));

			FVector Positions[3];
			FVector2D UVs[3];


			Positions[ReverseVertices ? 0 : 2] = ActorToWorld.TransformPosition(Polygon.Vertices[0]) - PostSub;
			UVs[ReverseVertices ? 0 : 2].X = (Positions[ReverseVertices ? 0 : 2] - TextureBase) | TextureX;
			UVs[ReverseVertices ? 0 : 2].Y = (Positions[ReverseVertices ? 0 : 2] - TextureBase) | TextureY;

			Positions[1] = ActorToWorld.TransformPosition(Polygon.Vertices[VertexIndex - 1]) - PostSub;
			UVs[1].X = (Positions[1] - TextureBase) | TextureX;
			UVs[1].Y = (Positions[1] - TextureBase) | TextureY;

			Positions[ReverseVertices ? 2 : 0] = ActorToWorld.TransformPosition(Polygon.Vertices[VertexIndex]) - PostSub;
			UVs[ReverseVertices ? 2 : 0].X = (Positions[ReverseVertices ? 2 : 0] - TextureBase) | TextureX;
			UVs[ReverseVertices ? 2 : 0].Y = (Positions[ReverseVertices ? 2 : 0] - TextureBase) | TextureY;

			for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
			{
				TempPositions.Add(Positions[CornerIndex]);
				OutMesh.WedgeTexCoords[0].Add(UVs[CornerIndex]);
			}
		}
	}

	// Merge vertices within a certain distance of each other.
	for (int32 i = 0; i < TempPositions.Num(); ++i)
	{
		FVector Position = TempPositions[i];
		int32 FinalIndex = INDEX_NONE;
		for (int32 VertexIndex = 0; VertexIndex < OutMesh.VertexPositions.Num(); ++VertexIndex)
		{
			if (FVerticesEqual(Position, OutMesh.VertexPositions[VertexIndex]))
			{
				FinalIndex = VertexIndex;
				break;
			}
		}
		if (FinalIndex == INDEX_NONE)
		{
			FinalIndex = OutMesh.VertexPositions.Add(Position);
		}
		OutMesh.WedgeIndices.Add(FinalIndex);
	}
#else
#if 0
	int32 NumPolys = pmxMeshInfoPtr->faseList.Num();

	// For each polygon in the model...
	TArray<FVector> TempPositions;
	for (int32 PolygonIndex = 0; PolygonIndex < NumPolys; ++PolygonIndex)
	{
		PMX_VERTEX& Polygon = pmxMeshInfoPtr->vertexList[PolygonIndex];

		UMaterialInterface*	Material = 0;// Polygon.Material;

		// Find a material index for this polygon.

		int32 MaterialIndex = OutMaterials.AddUnique(Material);

		// Cache the texture coordinate system for this polygon.

		FVector	TextureBase = Polygon.Position - (/*Brush ? Brush->GetPrePivot() : */FVector::ZeroVector),
			TextureX = Polygon.UV.X / UModel::GetGlobalBSPTexelScale(),
			TextureY = Polygon.UV.Y / UModel::GetGlobalBSPTexelScale();

		// For each vertex after the first two vertices...
		for (int32 VertexIndex = 2; VertexIndex < Polygon.Vertices.Num(); VertexIndex++)
		{
			// Create a triangle for the vertex.
			OutMesh.FaceMaterialIndices.Add(MaterialIndex);

			// Generate different smoothing mask for each poly to give the mesh hard edges.  Note: only 32 smoothing masks supported.
			OutMesh.FaceSmoothingMasks.Add(1 << (PolygonIndex % 32));

			FVector Positions[3];
			FVector2D UVs[3];


			Positions[ReverseVertices ? 0 : 2] = ActorToWorld.TransformPosition(Polygon.Vertices[0]) - PostSub;
			UVs[ReverseVertices ? 0 : 2].X = (Positions[ReverseVertices ? 0 : 2] - TextureBase) | TextureX;
			UVs[ReverseVertices ? 0 : 2].Y = (Positions[ReverseVertices ? 0 : 2] - TextureBase) | TextureY;

			Positions[1] = ActorToWorld.TransformPosition(Polygon.Vertices[VertexIndex - 1]) - PostSub;
			UVs[1].X = (Positions[1] - TextureBase) | TextureX;
			UVs[1].Y = (Positions[1] - TextureBase) | TextureY;

			Positions[ReverseVertices ? 2 : 0] = ActorToWorld.TransformPosition(Polygon.Vertices[VertexIndex]) - PostSub;
			UVs[ReverseVertices ? 2 : 0].X = (Positions[ReverseVertices ? 2 : 0] - TextureBase) | TextureX;
			UVs[ReverseVertices ? 2 : 0].Y = (Positions[ReverseVertices ? 2 : 0] - TextureBase) | TextureY;

			for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
			{
				TempPositions.Add(Positions[CornerIndex]);
				OutMesh.WedgeTexCoords[0].Add(UVs[CornerIndex]);
			}
		}
	}
#endif

	int32 matIndx = 0;
	int32 facecount = 0;
	for (int32 i = 0; i < pmxMeshInfoPtr->faseList.Num(); ++i)
	{
		UMaterialInterface*	Material = 0;// Polygon.Material;
		int32 MaterialIndex = 0;
		if (matIndx < pmxMeshInfoPtr->materialList.Num() && matIndx < OutMaterials.Num())
		{
			Material = OutMaterials[matIndx];
			MaterialIndex = matIndx;
		}

		if (!Material)
		{
			// Find a material index for this polygon.
			MaterialIndex = OutMaterials.AddUnique(Material);
		}

		OutMesh.FaceMaterialIndices.Add(MaterialIndex);
		OutMesh.FaceSmoothingMasks.Add(1 << (i % 32));
		if (!ReverseVertices)
		{
			for (int k = 0; k < 3; ++k)
			{
				//int k = 2;
				OutMesh.WedgeIndices.Add(pmxMeshInfoPtr->faseList[i].VertexIndex[k]);
				OutMesh.WedgeTexCoords[0].Add(pmxMeshInfoPtr->vertexList[pmxMeshInfoPtr->faseList[i].VertexIndex[k]].UV);
				FVector3f TangentZ = pmxMeshInfoPtr->vertexList[pmxMeshInfoPtr->faseList[i].VertexIndex[k]].Normal;
				OutMesh.WedgeTangentZ.Add(TangentZ.GetSafeNormal());
				facecount++;
			}
		}
		else
		{
			for (int k = 2; k > -1; --k)
			{
				OutMesh.WedgeIndices.Add(pmxMeshInfoPtr->faseList[i].VertexIndex[k]);
				OutMesh.WedgeTexCoords[0].Add(pmxMeshInfoPtr->vertexList[pmxMeshInfoPtr->faseList[i].VertexIndex[k]].UV);
				FVector3f TangentZ = pmxMeshInfoPtr->vertexList[pmxMeshInfoPtr->faseList[i].VertexIndex[k]].Normal;
				OutMesh.WedgeTangentZ.Add(TangentZ.GetSafeNormal());
				facecount++;
			}
		}
		if (Material)
		{
			if (facecount >= pmxMeshInfoPtr->materialList[matIndx].MaterialFaceNum)
			{
				matIndx++;
				facecount = 0;
			}
		}
	}
	// Merge vertices within a certain distance of each other.
	for (int32 i = 0; i < pmxMeshInfoPtr->vertexList.Num(); ++i)
	{
		FVector3f Position = pmxMeshInfoPtr->vertexList[i].Position;
		OutMesh.VertexPositions.Add(Position);
	}
#endif
}


/**
* Creates a static mesh object from raw triangle data.
*/

UStaticMesh* UMyUObjectTestClsFactory::CreateStaticMesh(
	struct FRawMesh& RawMesh,
	TArray<UMaterialInterface*>& Materials,
	UObject* InOuter,
	FName InName
	)
{
	// Create the UStaticMesh object.
	//FStaticMeshComponentRecreateRenderStateContext RecreateRenderStateContext(FindObject<UStaticMesh>(InOuter, *InName.ToString()));
	auto StaticMesh = NewObject<UStaticMesh>(InOuter, InName, RF_Public | RF_Standalone);

	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, StaticMesh);

	// Add one LOD for the base mesh
	FStaticMeshSourceModel* SrcModel = new((TArray<FStaticMeshSourceModel>&)StaticMesh->GetSourceModels()) FStaticMeshSourceModel();
	SrcModel->RawMeshBulkData->SaveRawMesh(RawMesh);
	//StaticMesh->Materials = Materials;

	for (int32 MatNum = 0; MatNum < Materials.Num(); ++MatNum)
	{
		StaticMesh->GetStaticMaterials().Add(FStaticMaterial());
		StaticMesh->GetStaticMaterials()[MatNum].MaterialInterface = Materials[MatNum];
	}

	int32 NumSections = StaticMesh->GetStaticMaterials().Num();

	// Set up the SectionInfoMap to enable collision
	for (int32 SectionIdx = 0; SectionIdx < NumSections; ++SectionIdx)
	{
		FMeshSectionInfo Info = StaticMesh->GetSectionInfoMap().Get(0, SectionIdx);
		Info.MaterialIndex = SectionIdx;
		Info.bEnableCollision = true;
		StaticMesh->GetSectionInfoMap().Set(0, SectionIdx, Info);
	}
	// @todo This overrides restored values currently 
	// but we need to be able to import over the existing settings 
	// if the user chose to do so.
	SrcModel->BuildSettings.bRemoveDegenerates = 1;//ImportOptions->bRemoveDegenerates;
	SrcModel->BuildSettings.bRecomputeNormals = 0;// ImportOptions->NormalImportMethod == FBXNIM_ComputeNormals;
	SrcModel->BuildSettings.bRecomputeTangents = 1;// ImportOptions->NormalImportMethod != FBXNIM_ImportNormalsAndTangents;
	
	//Test ? unkown
	SrcModel->BuildSettings.bGenerateLightmapUVs = false;
	SrcModel->BuildSettings.SrcLightmapIndex = 1;
	SrcModel->BuildSettings.DstLightmapIndex = 1;
	
	bool buildFlag = true;
	//buildFlag = false;
	StaticMesh->Build(buildFlag);
	StaticMesh->MarkPackageDirty();
	return StaticMesh;
}
/////////////////////////////////////////////////////////////////
AActor* UMyUObjectTestClsFactory::ConvertBrushesToStaticMesh(
	UObject * InParent,
	const FString& InStaticMeshPackageName, 
	//TArray<ABrush*>& InBrushesToConvert,
	MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
	const FVector& InPivotLocation,
	TArray<UMaterialInterface*>& Materials
	)
{
	AActor* NewActor(NULL);
#if 1 //pattern 2, asset not root
	FString StaticMeshName = "SM_" + InStaticMeshPackageName;
	// Make sure we have a parent
	if (!ensure(InParent))
	{
		return NULL;
	}


	FString BasePackageName
		= FPackageName::GetLongPackagePath(
			InParent->GetOutermost()->GetName()) / StaticMeshName;
	FString StaticMeshPackageName;

	FAssetToolsModule& AssetToolsModule
		= FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(
		BasePackageName, TEXT(""),
		StaticMeshPackageName, StaticMeshName);
	UPackage* Pkg = CreatePackage( *StaticMeshPackageName);

	FName ObjName = *FPackageName::GetLongPackageAssetName(StaticMeshName);
			//*StaticMeshPackageName;
	/*
	BasePackageName = PackageTools::SanitizePackageName(BasePackageName);

	// The material could already exist in the project
	FName ObjectPath = *(BasePackageName + TEXT(".") + ObjName);
	const FString Suffix(TEXT(""));
	FAssetToolsModule& AssetToolsModule 
		= FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	FString FinalPackageName;
	AssetToolsModule.Get().CreateUniqueAssetName(BasePackageName, Suffix, FinalPackageName, ObjName);
	UPackage* Pkg = CreatePackage(NULL, *FinalPackageName);
	////////////////////////////////
	*/
	/*FString FilePath = FPackageName::GetLongPackagePath(InParent->GetOutermost()->GetName())
		/ InStaticMeshPackageName;*/
	/*
	FString FilePath = "/Game/__" +InStaticMeshPackageName;
	FName ObjName = *FPackageName::GetLongPackageAssetName(FilePath);
	UPackage* Pkg = CreatePackage(NULL, *FilePath);
	check(Pkg != nullptr);
	*/
#else
	FName ObjName = *FPackageName::GetLongPackageAssetName(InStaticMeshPackageName)
	UPackage* Pkg = CreatePackage(NULL, *InStaticMeshPackageName);;
#endif

	check(Pkg != nullptr);


	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	/*
	for (int32 BrushesIdx = 0; BrushesIdx < InBrushesToConvert.Num(); ++BrushesIdx)
	{
		// Cache the location and rotation.
		Location = InBrushesToConvert[BrushesIdx]->GetActorLocation();
			Rotation = InBrushesToConvert[BrushesIdx]->GetActorRotation();

		// Leave the actor's rotation but move it to origin so the Static Mesh will generate correctly.
		InBrushesToConvert[BrushesIdx]->TeleportTo(Location - InPivotLocation, Rotation, false, true);
	}

	GEditor->RebuildModelFromBrushes(ConversionTempModel, true);
	GEditor->bspBuildFPolys(ConversionTempModel, true, 0);
	*/

	if (0 < 1)//ConversionTempModel->Polys->Element.Num())
	{
		UStaticMesh* NewMesh 
			= CreateStaticMeshFromBrush(
				Pkg, 
				ObjName,
				pmxMeshInfoPtr, 
				Materials
				/*, NULL, ConversionTempModel*/
				);
		/*
		NewActor = FActorFactoryAssetProxy::AddActorForAsset(NewMesh);
		
		NewActor->Modify();

		NewActor->InvalidateLightingCache();
		NewActor->PostEditChange();
		NewActor->PostEditMove(true);
		NewActor->Modify();
		GEditor->Layers->InitializeNewActorLayers(NewActor);

		// Teleport the new actor to the old location but not the old rotation. The static mesh is built to the rotation already.
		NewActor->TeleportTo(InPivotLocation, FRotator(0.0f, 0.0f, 0.0f), false, true);

		// Destroy the old brushes.
		for (int32 BrushIdx = 0; BrushIdx < InBrushesToConvert.Num(); ++BrushIdx)
		{
			GEditor->Layers->DisassociateActorFromLayers(InBrushesToConvert[BrushIdx]);
			GWorld->EditorDestroyActor(InBrushesToConvert[BrushIdx], true);
		}
		*/
		// Notify the asset registry
		FAssetRegistryModule::AssetCreated(NewMesh);
	}

	/*
	ConversionTempModel->EmptyModel(1, 1);

	GEditor->RedrawLevelEditingViewports();
	*/
	return NewActor;
}
#endif