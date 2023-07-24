// Copyright 2015 BlackMa9. All Rights Reserved.
//see S:\UE5SRC\EpicUE5\UE_5.0\Engine\Source\Editor\UnrealEd\Private\Fbx\FbxSkeletalMeshImport.cpp
#include "PmxFactory.h"
#include "IM4UPrivatePCH.h"

#include "CoreMinimal.h"
#include "Factories.h"
#include "BusyCursor.h"
#include "SSkeletonWidget.h"

//#include "FbxImporter.h"

//#include "Misc/FbxErrors.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"

/////////////////////////

#include "Engine.h"
#include "Editor.h"
#include "TextureLayout.h"
#include "ImportUtils/SkelImport.h"
#include "ImportUtils/SkeletalMeshImportUtils.h"
//#include "FbxImporter.h"
#include "AnimEncoding.h"
#include "SSkeletonWidget.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetNotifications.h"

#include "ObjectTools.h"

#include "ApexClothingUtils.h"
#include "Developer/MeshUtilities/Public/MeshUtilities.h"

#include "MessageLogModule.h"
#include "ComponentReregisterContext.h"

#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "Developer/PhysicsUtilities/Public/PhysicsAssetUtils.h"

////////////

#include "PmdImporter.h"
#include "PmxImporter.h"
#include "PmxImportUI.h"

#include "MMDSkeletalMeshImportData.h"
#include "MMDStaticMeshImportData.h"

#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshModel.h"



#define LOCTEXT_NAMESPACE "PMXImpoter"

DEFINE_LOG_CATEGORY(LogMMD4UE4_PMXFactory)

/////////////////////////////////////////////////////////
// 3 "ProcessImportMesh..." functions outputing Unreal data from a filled FSkeletalMeshBinaryImport
// and a handfull of other minor stuff needed by these 
// Fully taken from SkeletalMeshImport.cpp


using namespace SkeletalMeshImportUtils;
using namespace MMD4UE4;
/////////////////////////////////////////////////////////

UPmxFactory::UPmxFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	SupportedClass = NULL;
	//SupportedClass = UPmxFactory::StaticClass();
	Formats.Empty();

	Formats.Add(TEXT("pmd;PMD meshes and animations"));
	Formats.Add(TEXT("pmx;PMX meshes and animations"));

	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	
	bOperationCanceled = false;
	bDetectImportTypeOnImport = false;

	//ImportUI = NewObject<UPmxImportUI>(this, NAME_None, RF_NoFlags);
}

void UPmxFactory::PostInitProperties()
{
	Super::PostInitProperties();

	ImportUI = NewObject<UPmxImportUI>(this, NAME_None, RF_NoFlags);
}

bool UPmxFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UPmxFactory::StaticClass());
}

UClass* UPmxFactory::ResolveSupportedClass()
{
	return UPmxFactory::StaticClass();
}

////////////////////////////////////////////////
//IM4U Develop Temp Define
//////////////////////////////////////////////
//#define DEBUG_MMD_UE4_ORIGINAL_CODE	(1)
#define DEBUG_MMD_PLUGIN_SKELTON	(1) 
//#define DEBUG_MMD_PLUGIN_STATICMESH	(1)
//#define DEBUG_MMD_PLUGIN_ANIMATION	(1)
//////////////////////////////////////////////


UObject* UPmxFactory::FactoryCreateBinary
	(
	UClass*			Class,
	UObject*			InParent,
	FName				Name,
	EObjectFlags		Flags,
	UObject*			Context,
	const TCHAR*		Type,
	const uint8*&		Buffer,
	const uint8*		BufferEnd,
	FFeedbackContext*	Warn,
	bool&				bOutOperationCanceled
	)
{
	// MMD Default 
	importAssetTypeMMD = E_MMD_TO_UE4_UNKOWN;
#ifdef DEBUG_MMD_PLUGIN_SKELTON
	importAssetTypeMMD = E_MMD_TO_UE4_SKELTON;
#endif
#ifdef DEBUG_MMD_PLUGIN_STATICMESH
	importAssetTypeMMD = E_MMD_TO_UE4_STATICMESH;
#endif

	if (bOperationCanceled)
	{
		bOutOperationCanceled = true;
		
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, NULL);
		return NULL;
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPreImport.Broadcast(this, Class, InParent, Name, Type);

	UObject* NewObject = NULL;


	FPmxImporter* PmxImporter = FPmxImporter::GetInstance();

	EPMXImportType ForcedImportType = PMXIT_StaticMesh;

	// For multiple files, use the same settings
	bDetectImportTypeOnImport = false;

	//judge MMD format(pmx or pmd)
	bool bIsPmxFormat = false;
	if (FString(Type).Equals(TEXT("pmx"), ESearchCase::IgnoreCase))
	{
		//Is PMX format 
		bIsPmxFormat = true;
	}
	//Load MMD Model From binary File
	MMD4UE4::PmxMeshInfo pmxMeshInfoPtr;
	pmxMaterialImportHelper.InitializeBaseValue(InParent);
	bool pmxImportResult = false;
	if (bIsPmxFormat)
	{
		//pmx ver
		pmxImportResult = pmxMeshInfoPtr.PMXLoaderBinary(Buffer, BufferEnd);
	}
	else
	{
		//pmd ver
		MMD4UE4::PmdMeshInfo PmdMeshInfo;
		if (PmdMeshInfo.PMDLoaderBinary(Buffer, BufferEnd))
		{
			//Up convert From PMD to PMX format gfor ue4
			pmxImportResult = PmdMeshInfo.ConvertToPmxFormat(&pmxMeshInfoPtr);
		}
	}
	if (!pmxImportResult)
	{
		// Log the error message and fail the import.
		Warn->Log(ELogVerbosity::Error, "PMX Import ERR...FLT");
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, NULL);
		return NULL;
	}
	else
	{
		//モデル読み込み後の警告文表示：コメント欄
		FText TitleStr = FText::Format(LOCTEXT("ImportReadMe_Generic", "{0}"), FText::FromString("tilesss"));
		const FText Message
			= FText::Format(LOCTEXT("ImportReadMe_Generic_Msg",
			"Important!! \nReadMe Lisence \n modele Name:\n'{0}'\n \n Model Comment JP:\n'{1}'"),
			FText::FromString(pmxMeshInfoPtr.modelNameJP), FText::FromString(pmxMeshInfoPtr.modelCommentJP));
		//if (EAppReturnType::Ok != FMessageDialog::Open(EAppMsgType::OkCancel, Message))
		//{
		//	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, NULL);
		//	return NULL;
		//}
		TitleStr = FText::Format(LOCTEXT("ImportReadMe_Generic_Dbg", "{0} 制限事項"), FText::FromString("IM4U Plugin"));
		const FText MessageDbg
			= FText(LOCTEXT("ImportReadMe_Generic_Dbg_Msg",
			"次のImportOption用Slateはまだ実装途中です。\n\
			Import対象はSkeletonのみ生成可能。\n\
			現時点で有効なパラメータは、表示されている項目が有効です。") ); 
		//FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
	}

	// show Import Option Slate
	bool bImportAll = false;
	ImportUI->bIsObjImport = true;//obj mode
	ImportUI->OriginalImportType = EPMXImportType::PMXIT_SkeletalMesh;
	PMXImportOptions* ImportOptions
		= GetImportOptions(
		PmxImporter,
		ImportUI,
		true,//bShowImportDialog, 
		InParent->GetPathName(),
		bOperationCanceled,
		bImportAll,
		ImportUI->bIsObjImport,//bIsPmxFormat,
		bIsPmxFormat,
		ForcedImportType
		);
	if (bImportAll)
	{
		// If the user chose to import all, we don't show the dialog again and use the same settings for each object until importing another set of files
		//bShowOption = false;
	}

	if (ImportOptions)
	{
		Warn->BeginSlowTask(NSLOCTEXT("PmxFactory", "BeginImportingPmxMeshTask", "Importing Pmx mesh"), true);
#if 1
		{

			// For animation and static mesh we assume there is at lease one interesting node by default
			int32 InterestingNodeCount = 1;

			if (importAssetTypeMMD == E_MMD_TO_UE4_SKELTON)
			{
#ifdef DEBUG_MMD_PLUGIN_SKELTON

				InterestingNodeCount = 1;//test ? not Anime?

#endif
			}
			else if (importAssetTypeMMD == E_MMD_TO_UE4_STATICMESH)
			{

			}


			if (InterestingNodeCount > 1)
			{
				// the option only works when there are only one asset
//				ImportOptions->bUsedAsFullName = false;
			}

			const FString Filename(UFactory::CurrentFilename);
			if (/*RootNodeToImport &&*/ InterestingNodeCount > 0)
			{
				int32 NodeIndex = 0;

				int32 ImportedMeshCount = 0;
				UStaticMesh* NewStaticMesh = NULL;
				if (importAssetTypeMMD == E_MMD_TO_UE4_STATICMESH)  // static mesh
				{

				}
				else if (importAssetTypeMMD == E_MMD_TO_UE4_SKELTON)// skeletal mesh
				{
#ifdef DEBUG_MMD_PLUGIN_SKELTON
					int32 TotalNumNodes = 0;

					//for (int32 i = 0; i < SkelMeshArray.Num(); i++)
					for (int32 i = 0; i < 1/*SkelMeshArray.Num()*/; i++)
					{
						int32 LODIndex=0;
#ifdef DEBUG_MMD_UE4_ORIGINAL_CODE
					
#else
						// for MMD?
						int32 MaxLODLevel = 1;
						FSkeletalMeshImportData smid;
						USkeletalMesh* NewMesh = NULL;
						if (LODIndex == 0 /*&& SkelMeshNodeArray.Num() != 0*/)
						{
							FName OutputName = FName(*FString::Printf(TEXT("%s"), *Name.ToString() ));// FbxImporter->MakeNameForMesh(Name.ToString(), SkelMeshNodeArray[0]);
							
							
							NewMesh = ImportSkeletalMesh(
								InParent,
								&pmxMeshInfoPtr,//SkelMeshNodeArray,
								OutputName,
								Flags,
								//ImportUI->SkeletalMeshImportData,
								FPaths::GetBaseFilename(Filename),
								&smid,// test for MMD,
								true
								);
							NewObject = NewMesh;
						}

						// import morph target
						if (NewMesh && ImportUI->SkeletalMeshImportData->bImportMorphTargets)
						{
							// Disable material importing when importing morph targets
							uint32 bImportMaterials = ImportOptions->bImportMaterials;
							ImportOptions->bImportMaterials = 0;

							ImportFbxMorphTarget(
								//SkelMeshNodeArray, 
								pmxMeshInfoPtr,
								NewMesh,
								InParent, 
								Filename, 
								LODIndex, smid
								);

							ImportOptions->bImportMaterials = !!bImportMaterials;
						}

						//add self
						if (NewObject)
						{
							//MMD Extend asset
							CreateMMDExtendFromMMDModel(
								InParent,
								//FName(*NewObject->GetName()),
								Cast<USkeletalMesh>(NewObject),
								&pmxMeshInfoPtr
								);

						}

						//end phese
						if (NewObject)
						{
							TotalNumNodes++;
							NodeIndex++;
							FFormatNamedArguments Args;
							Args.Add(TEXT("NodeIndex"), NodeIndex);
							Args.Add(TEXT("ArrayLength"), 1);// SkelMeshArray.Num());
							GWarn->StatusUpdate(NodeIndex, 1/*SkelMeshArray.Num()*/, FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
						}
#endif
					}


					// if total nodes we found is 0, we didn't find anything. 
					if (TotalNumNodes == 0)
					{
						AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_NoMeshFoundOnRoot", "Could not find any valid mesh on the root hierarchy. If you have mesh in the sub hierarchy, please enable option of [Import Meshes In Bone Hierarchy] when import.")),
							"FFbxErrors::SkeletalMesh_NoMeshFoundOnRoot");
					}
#endif
				}
			}
			else
			{
#if 1//DEBUG_MMD_UE4_ORIGINAL_CODE
				if (importAssetTypeMMD == E_MMD_TO_UE4_SKELTON)
				{
					AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_InvalidBone", "Failed to find any bone hierarchy. Try disabling the \"Import As Skeletal\" option to import as a rigid mesh. ")),"FFbxErrors::SkeletalMesh_InvalidBone");
				}
				else
				{
					AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_InvalidNode", "Could not find any node.")),"FFbxErrors::SkeletalMesh_InvalidNode");
				}
#endif
			}
		}

		if (NewObject == NULL)
		{
			AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_NoObject", "Import failed.")),"FFbxErrors::Generic_ImportingNewObjectFailed");
			Warn->Log(ELogVerbosity::Error, "PMX Import ERR [NewObject is NULL]...FLT");
		}

		//FbxImporter->ReleaseScene();
#endif
		Warn->EndSlowTask();
	}
	else
	{
		const FText Message 
			= FText::Format(LOCTEXT("ImportFailed_Generic", 
				"Failed to import '{0}'. Failed to create asset '{1}'\nMMDモデルの読み込みを中止します。\nIM4U Plugin"),
				FText::FromString(*Name.ToString()), FText::FromString(*Name.ToString()));
		FMessageDialog::Open(EAppMsgType::Ok, Message);
		//UE_LOG(LogAssetTools, Warning, TEXT("%s"), *Message.ToString());
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, NewObject);

	return NewObject;
}


//////////////////////////////////////////////////////////////////////
USkeletalMesh* UPmxFactory::ImportSkeletalMesh(
	UObject* InParent,
	MMD4UE4::PmxMeshInfo *pmxMeshInfoPtr,
	const FName& Name,
	EObjectFlags Flags,
	//UFbxSkeletalMeshImportData* TemplateImportData,
	FString Filename,
	//TArray<FbxShape*> *FbxShapeArray,
	FSkeletalMeshImportData* OutData,
	bool bCreateRenderData
	)
{
	bool bDiffPose;
	int32 SkelType = 0; // 0 for skeletal mesh, 1 for rigid mesh

	//bool bCreateRenderData = true;
	struct ExistingSkelMeshData* ExistSkelMeshDataPtr = NULL;

	if (true /*!FbxShapeArray*/)
	{
		//UObject* ExistingObject = StaticFindObjectFast(UObject::StaticClass(), InParent, *Name.ToString(), false, false, RF_PendingKill);//~UE4.10
		UObject* ExistingObject = StaticFindObjectFast(UObject::StaticClass(), InParent, *Name.ToString(), false, false, EObjectFlags::RF_NoFlags, EInternalObjectFlags::Garbage);//UE4.11~
		USkeletalMesh* ExistingSkelMesh = Cast<USkeletalMesh>(ExistingObject);

		if (ExistingSkelMesh)
		{

			ExistingSkelMesh->PreEditChange(NULL);
			//ExistSkelMeshDataPtr = SaveExistingSkelMeshData(ExistingSkelMesh);
		}
		// if any other object exists, we can't import with this name
		else if (ExistingObject)
		{
			AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("FbxSkeletaLMeshimport_OverlappingName", "Same name but different class: '{0}' already exists"), FText::FromString(ExistingObject->GetName()))),"FFbxErrors::Generic_SameNameAssetExists");
			return NULL;
		}
	}

	// [from USkeletalMeshFactory::FactoryCreateBinary]
	USkeletalMesh* SkeletalMesh = NewObject<USkeletalMesh>(InParent, Name, Flags);

	SkeletalMesh->PreEditChange(NULL);

	FSkeletalMeshImportData TempData;
	// Fill with data from buffer - contains the full .FBX file. 	
	FSkeletalMeshImportData* SkelMeshImportDataPtr = &TempData;
	if (OutData)
	{
		SkelMeshImportDataPtr = OutData;
	}

	/*Import Bone Start*/
	bool bUseTime0AsRefPose = false;// ImportOptions->bUseT0AsRefPose;
	// Note: importing morph data causes additional passes through this function, so disable the warning dialogs
	// from popping up again on each additional pass.  
	if (
		!ImportBone(
			//NodeArray, 
			pmxMeshInfoPtr,
			*SkelMeshImportDataPtr,
			//TemplateImportData, 
			//SortedLinkArray,
			bDiffPose, 
			false,//(FbxShapeArray != NULL),
			bUseTime0AsRefPose)
			
			/*false*/
		)
	{
		AddTokenizedErrorMessage(FTokenizedMessage::Create(
			EMessageSeverity::Error, 
			LOCTEXT("FbxSkeletaLMeshimport_MultipleRootFound", "Multiple roots found")),"FFbxErrors::SkeletalMesh_MultipleRoots");
		// I can't delete object here since this is middle of import
		// but I can move to transient package, and GC will automatically collect it
		SkeletalMesh->ClearFlags(RF_Standalone);
		SkeletalMesh->Rename(NULL, GetTransientPackage());
		return NULL;
	}
	/*Import Bone End*/

	/*
	// Inport  Material @@@@@

	// Create a list of all unique fbx materials.  This needs to be done as a separate pass before reading geometry
	// so that we know about all possible materials before assigning material indices to each triangle
	TArray<FbxSurfaceMaterial*> FbxMaterials;
	for (int32 NodeIndex = 0; NodeIndex < NodeArray.Num(); ++NodeIndex)
	{
		Node = NodeArray[NodeIndex];

		int32 MaterialCount = Node->GetMaterialCount();

		for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
		{
			FbxSurfaceMaterial* FbxMaterial = Node->GetMaterial(MaterialIndex);
			if (!FbxMaterials.Contains(FbxMaterial))
			{
				FbxMaterials.Add(FbxMaterial);
				// Add an entry for each unique material
				SkelMeshImportDataPtr->Materials.Add(VMaterial());
			}
		}
	}
	*/

	for (int32 MaterialIndex = 0; MaterialIndex < pmxMeshInfoPtr->materialList.Num(); ++MaterialIndex)
	{
		// Add an entry for each unique material
		SkeletalMeshImportData::FMaterial NewMaterial;
		SkelMeshImportDataPtr->Materials.Add(NewMaterial);
	}
	//SkelMeshImportDataPtr->MaxMaterialIndex= pmxMeshInfoPtr->materialList.Num();
	/*
	for (int32 NodeIndex = 0; NodeIndex < NodeArray.Num(); ++NodeIndex)
	{
		Node = NodeArray[NodeIndex];
		FbxMesh = Node->GetMesh();
		FbxSkin* Skin = (FbxSkin*)FbxMesh->GetDeformer(0, FbxDeformer::eSkin);
		FbxShape* FbxShape = NULL;
		if (FbxShapeArray)
		{
			FbxShape = (*FbxShapeArray)[NodeIndex];
		}

		// NOTE: This function may invalidate FbxMesh and set it to point to a an updated version
		if (!FillSkelMeshImporterFromFbx(*SkelMeshImportDataPtr, FbxMesh, Skin, FbxShape, SortedLinkArray, FbxMaterials))
		{
			// I can't delete object here since this is middle of import
			// but I can move to transient package, and GC will automatically collect it
			SkeletalMesh->ClearFlags(RF_Standalone);
			SkeletalMesh->Rename(NULL, GetTransientPackage());
			return NULL;
		}

		if (bUseTime0AsRefPose && bDiffPose)
		{
			// deform skin vertex to the frame 0 from bind pose
			SkinControlPointsToPose(*SkelMeshImportDataPtr, FbxMesh, FbxShape, true);
		}
	}
	*/
	// NOTE: This function may invalidate FbxMesh and set it to point to a an updated version
	if (
		!FillSkelMeshImporterFromFbx(
			*SkelMeshImportDataPtr,
			pmxMeshInfoPtr
			/*
			FbxMesh,
			Skin, 
			FbxShape,
			SortedLinkArray, 
			FbxMaterials*/
			)
		)
		/*false*/
	{
		// I can't delete object here since this is middle of import
		// but I can move to transient package, and GC will automatically collect it
		SkeletalMesh->ClearFlags(RF_Standalone);
		SkeletalMesh->Rename(NULL, GetTransientPackage());
		return NULL;
	}

	// reorder material according to "SKinXX" in material name
	/*SetMaterialSkinXXOrder(*SkelMeshImportDataPtr);
*/
	if (/*ImportOptions->bPreserveSmoothingGroups*/
		false//true
		)
	{
		//DoUnSmoothVerts(*SkelMeshImportDataPtr);
	}
	else
	{
		SkelMeshImportDataPtr->PointToRawMap.AddUninitialized(SkelMeshImportDataPtr->Points.Num());
		for (int32 PointIdx = 0; PointIdx<SkelMeshImportDataPtr->Points.Num(); PointIdx++)
		{
			SkelMeshImportDataPtr->PointToRawMap[PointIdx] = PointIdx;
		}
	}

	// process materials from import data
	ProcessImportMeshMaterials(SkeletalMesh->GetMaterials(), *SkelMeshImportDataPtr);

	// process reference skeleton from import data
	int32 SkeletalDepth = 0;
	if (!ProcessImportMeshSkeleton(SkeletalMesh->GetSkeleton(), SkeletalMesh->GetRefSkeleton(), SkeletalDepth, *SkelMeshImportDataPtr))
	{
		SkeletalMesh->ClearFlags(RF_Standalone);
		SkeletalMesh->Rename(NULL, GetTransientPackage());
		return NULL;
	}
	UE_LOG(LogMMD4UE4_PMXFactory, Warning, TEXT("Bones digested - %i  Depth of hierarchy - %i"), SkeletalMesh->GetRefSkeleton().GetNum(), SkeletalDepth);

	// process bone influences from import data
	SkeletalMeshImportUtils::ProcessImportMeshInfluences(*SkelMeshImportDataPtr,L"MMDMeshName");


	SkeletalMesh->PreEditChange(NULL);
	//Dirty the DDC Key for any imported Skeletal Mesh
	SkeletalMesh->InvalidateDeriveDataCacheGUID();

	FSkeletalMeshModel* ImportedResource = SkeletalMesh->GetImportedModel();
	check(ImportedResource->LODModels.Num() == 0);
	ImportedResource->LODModels.Empty();
	ImportedResource->LODModels.Add(new FSkeletalMeshLODModel());
	const int32 ImportLODModelIndex = 0;
	FSkeletalMeshLODModel& LODModel = ImportedResource->LODModels[ImportLODModelIndex];

	SkeletalMesh->SaveLODImportedData(0, *SkelMeshImportDataPtr);

	{
		//We reimport both
		SkeletalMesh->SetLODImportedDataVersions(ImportLODModelIndex, ESkeletalMeshGeoImportVersions::LatestVersion, ESkeletalMeshSkinningImportVersions::LatestVersion);
	}
	SkeletalMesh->ResetLODInfo();
	FSkeletalMeshLODInfo & NewLODInfo = SkeletalMesh->AddLODInfo();
	NewLODInfo.ReductionSettings.NumOfTrianglesPercentage = 1.0f;
	NewLODInfo.ReductionSettings.NumOfVertPercentage = 1.0f;
	NewLODInfo.ReductionSettings.MaxDeviationPercentage = 0.0f;
	NewLODInfo.LODHysteresis = 0.02f;

	// Create initial bounding box based on expanded version of reference pose for meshes without physics assets. Can be overridden by artist.
	FBox3f BoundingBox(SkelMeshImportDataPtr->Points.GetData(), SkelMeshImportDataPtr->Points.Num());
	FBox3f Temp = BoundingBox;
	FVector3f MidMesh = 0.5f*(Temp.Min + Temp.Max);
	BoundingBox.Min = Temp.Min + 1.0f*(Temp.Min - MidMesh);
	BoundingBox.Max = Temp.Max + 1.0f*(Temp.Max - MidMesh);
	// Tuck up the bottom as this rarely extends lower than a reference pose's (e.g. having its feet on the floor).
	// Maya has Y in the vertical, other packages have Z.
	//BEN const int32 CoordToTuck = bAssumeMayaCoordinates ? 1 : 2;
	//BEN BoundingBox.Min[CoordToTuck]	= Temp.Min[CoordToTuck] + 0.1f*(Temp.Min[CoordToTuck] - MidMesh[CoordToTuck]);
	BoundingBox.Min[2] = Temp.Min[2] + 0.1f*(Temp.Min[2] - MidMesh[2]);
#if 0 /* under ~ UE4.11 */
	SkeletalMesh->Bounds = FBoxSphereBounds(BoundingBox);
#else /* over UE4.12 ~*/
	SkeletalMesh->SetImportedBounds( FBoxSphereBounds(FBox(BoundingBox)) );
#endif

	// Store whether or not this mesh has vertex colors
	SkeletalMesh->SetHasVertexColors(SkelMeshImportDataPtr->bHasVertexColors);
	SkeletalMesh->SetVertexColorGuid(SkeletalMesh->GetHasVertexColors() ? FGuid::NewGuid() : FGuid());
	// Release the static mesh's resources.
	SkeletalMesh->ReleaseResources();

	// Flush the resource release commands to the rendering thread to ensure that the build doesn't occur while a resource is still
	// allocated, and potentially accessing the UStaticMesh.
	SkeletalMesh->ReleaseResourcesFence.Wait();



	// Pass the number of texture coordinate sets to the LODModel.  Ensure there is at least one UV coord
	LODModel.NumTexCoords = FMath::Max<uint32>(1, SkelMeshImportDataPtr->NumTexCoords);
#if 1
	if (bCreateRenderData)
	{
		TArray<FVector3f> LODPoints;
		TArray<SkeletalMeshImportData::FMeshWedge> LODWedges;
		TArray<SkeletalMeshImportData::FMeshFace> LODFaces;
		TArray<SkeletalMeshImportData::FVertInfluence> LODInfluences;
		TArray<int32> LODPointToRawMap;
		SkelMeshImportDataPtr->CopyLODImportData(LODPoints, LODWedges, LODFaces, LODInfluences, LODPointToRawMap);

		const bool bShouldComputeNormals = true/*!ImportOptions->ShouldImportNormals()*/ || !SkelMeshImportDataPtr->bHasNormals;
		const bool bShouldComputeTangents = true/*!ImportOptions->ShouldImportTangents()*/ || !SkelMeshImportDataPtr->bHasTangents;

		IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");

		TArray<FText> WarningMessages;
		TArray<FName> WarningNames;
		// Create actual rendering data.
#if 0 /* under ~ UE4.10 */
		if (!MeshUtilities.BuildSkeletalMesh(
			ImportedResource->LODModels[0], 
			SkeletalMesh->GetRefSkeleton(),
			LODInfluences,
			LODWedges,
			LODFaces,
			LODPoints, 
			LODPointToRawMap,
			false,//ImportOptions->bKeepOverlappingVertices, 
			bShouldComputeNormals, 
			bShouldComputeTangents,
			&WarningMessages, 
			&WarningNames)
			)
#else /* UE4.11~ over */
		if (!MeshUtilities.BuildSkeletalMesh(
			ImportedResource->LODModels[0],"MMDMeshName",
			SkeletalMesh->GetRefSkeleton(),
			LODInfluences,
			LODWedges,
			LODFaces,
			LODPoints,
			LODPointToRawMap)
			)
#endif
		{
			if (WarningNames.Num() == WarningMessages.Num())
			{
				// temporary hack of message/names, should be one token or a struct
				for (int32 MessageIdx = 0; MessageIdx<WarningMessages.Num(); ++MessageIdx)
				{
					AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, WarningMessages[MessageIdx]), WarningNames[MessageIdx]);
				}
			}

			SkeletalMesh->MarkAsGarbage();
			return NULL;
		}
		else if (WarningMessages.Num() > 0)
		{
			// temporary hack of message/names, should be one token or a struct
			if (WarningNames.Num() == WarningMessages.Num())
			{
				// temporary hack of message/names, should be one token or a struct
				for (int32 MessageIdx = 0; MessageIdx<WarningMessages.Num(); ++MessageIdx)
				{
					AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, WarningMessages[MessageIdx]), WarningNames[MessageIdx]);
				}
			}
		}

		// Presize the per-section shadow casting array with the number of sections in the imported LOD.
		const int32 NumSections = LODModel.Sections.Num();
		for (int32 SectionIndex = 0; SectionIndex < NumSections; ++SectionIndex)
		{
			SkeletalMesh->GetLODInfo(0)->LODMaterialMap.Add(SectionIndex);
		}

		if (ExistSkelMeshDataPtr)
		{
//			RestoreExistingSkelMeshData(ExistSkelMeshDataPtr, SkeletalMesh);
		}

		// Store the current file path and timestamp for re-import purposes
		/*UFbxSkeletalMeshImportData* ImportData = UFbxSkeletalMeshImportData::GetImportDataForSkeletalMesh(SkeletalMesh, TemplateImportData);
		SkeletalMesh->AssetImportData->SourceFilePath = FReimportManager::SanitizeImportFilename(UFactory::CurrentFilename, SkeletalMesh);
		SkeletalMesh->AssetImportData->SourceFileTimestamp = IFileManager::Get().GetTimeStamp(*UFactory::CurrentFilename).ToString();
		SkeletalMesh->AssetImportData->bDirty = false;
		*/
		SkeletalMesh->GetAssetImportData()->Update(UFactory::CurrentFilename);

		SkeletalMesh->CalculateInvRefMatrices();
		SkeletalMesh->PostEditChange();
		SkeletalMesh->MarkPackageDirty();

		// Now iterate over all skeletal mesh components re-initialising them.
		for (TObjectIterator<USkeletalMeshComponent> It; It; ++It)
		{
			USkeletalMeshComponent* SkelComp = *It;
			if (SkelComp->GetSkeletalMeshAsset() == SkeletalMesh)
			{
				FComponentReregisterContext ReregisterContext(SkelComp);
			}
		}
	}
#endif
#if 1 //phy
	if (InParent != GetTransientPackage())
	{
		// Create PhysicsAsset if requested and if physics asset is null
		if (ImportUI->bCreatePhysicsAsset)
		{
			if (SkeletalMesh->GetPhysicsAsset() == NULL)
			{
				FString ObjectName = FString::Printf(TEXT("%s_PhysicsAsset"), *SkeletalMesh->GetName());
				UPhysicsAsset * NewPhysicsAsset = CreateAsset<UPhysicsAsset>(InParent->GetName(), ObjectName, true); 
				if (!NewPhysicsAsset)
				{
					AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("FbxSkeletaLMeshimport_CouldNotCreatePhysicsAsset", "Could not create Physics Asset ('{0}') for '{1}'"), FText::FromString(ObjectName), FText::FromString(SkeletalMesh->GetName()))),"FFbxErrors::SkeletalMesh_FailedToCreatePhyscisAsset");
				}
				else
				{
					FPhysAssetCreateParams NewBodyData; 
					//PmxPhysicsParam pmxpm;
					NewBodyData.bDisableCollisionsByDefault = true;
					NewBodyData.MinBoneSize = 2;
					//NewBodyData.pMmdParam = &pmxpm;
					FText CreationErrorMessage;
					bool bSuccess
						= FPhysicsAssetUtils::CreateFromSkeletalMesh(
								NewPhysicsAsset, 
								SkeletalMesh, 
								NewBodyData, 
								CreationErrorMessage
								);
					if (!bSuccess)
					{
						AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, CreationErrorMessage),"FFbxErrors::SkeletalMesh_FailedToCreatePhyscisAsset");
						// delete the asset since we could not have create physics asset
						TArray<UObject*> ObjectsToDelete;
						ObjectsToDelete.Add(NewPhysicsAsset);
						ObjectTools::DeleteObjects(ObjectsToDelete, false);
					}
					//NewPhysicsAsset->PreEditChange(NULL);

					int bdn = NewPhysicsAsset->SkeletalBodySetups.Num(); 
					for (int i = 0; i < bdn; i++)
					{
						TObjectPtr<USkeletalBodySetup>& bd = NewPhysicsAsset->SkeletalBodySetups[i];
						USkeletalBodySetup* pbd = bd.Get();
						FName bname = pbd->BoneName;// pbd->GetFName();
						TArray<PMX_RIGIDBODY> rbs = pmxMeshInfoPtr->findRigid(bname);
						
						
						
						FKAggregateGeom& ag = pbd->AggGeom;
						
						ag.EmptyElements();
						if (rbs.Num()<1) {
							FKBoxElem ke(0.5,0.5,1);
							
							ag.BoxElems.Add(ke);
							pbd->PhysicsType = PhysType_Kinematic; 
							pbd->CollisionReponse = EBodyCollisionResponse::BodyCollision_Disabled;
							
							ke.SetContributeToMass(false);
							//pbd->BoneName = L"";
							continue;
						}
						
						for (auto rb:rbs) {
							
							if (rb.Mass < 0.001 || rb.RigidBodyGroupIndex < 3)
							{
								pbd->PhysicsType = PhysType_Kinematic;

							}
							else
							{
								
							}
							FTransform ft;
							ft.SetLocation((FVector)rb.Position);
							//FQuat qd  = FRotator(rb.Rotation.X * 180.f / PI, rb.Rotation.Y * 180.f / PI, rb.Rotation.Z * 180.f / PI).Quaternion();

							ft.SetRotation(rb.Quat);

							if (rb.ShapeType == 0) {

								FKSphereElem ke(rb.Size.X);
								
								ke.SetTransform(ft);
								ke.SetContributeToMass(true);
								//pbd->CalculateMass();
								ag.SphereElems.Add(ke);
							}
							else if (rb.ShapeType == 1) {

								FKBoxElem ke(rb.Size.X , rb.Size.Z , rb.Size.Y );

								//ft.SetRotation(qd);
								ke.SetTransform(ft);
								ke.SetContributeToMass(true);

								ag.BoxElems.Add(ke);
							}
							else if (rb.ShapeType == 2) {

								FKSphylElem ke(rb.Size.X, rb.Size.Y);

								ke.SetTransform(ft);
								ke.SetContributeToMass(true);

								ag.SphylElems.Add(ke);
							}
						}

					}

#if 1
					for (int i = 0; i < bdn; i++)
						for (int j = i + 1; j < bdn; j++)
							if (NewPhysicsAsset->SkeletalBodySetups[i]->PhysicsType != NewPhysicsAsset->SkeletalBodySetups[j]->PhysicsType)
								NewPhysicsAsset->EnableCollision(j, i);
							//else						NewPhysicsAsset->DisableCollision(j, i);
#endif
					//NewPhysicsAsset->ConstraintSetup.Empty();
					int csn=NewPhysicsAsset->ConstraintSetup.Num();


					for (int i = 0; i < csn; i++)
					{
						NewPhysicsAsset->ConstraintSetup[i]->PreEditChange(NULL);
						FConstraintInstance &cs = NewPhysicsAsset->ConstraintSetup[i]->DefaultInstance;
						//cs.ProfileInstance.AngularDrive.AngularDriveMode = EAngularDriveMode::TwistAndSwing;
						cs.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Limited;
						cs.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Limited;
						cs.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Limited;
						cs.ProfileInstance.TwistLimit.TwistLimitDegrees = 5;
						cs.ProfileInstance.ConeLimit.Swing1LimitDegrees = 5;
						cs.ProfileInstance.ConeLimit.Swing2LimitDegrees = 5;
						auto s1 = cs.ProfileInstance.ConeLimit.Swing1Motion;
						auto s2 = cs.ProfileInstance.ConeLimit.Swing1LimitDegrees;
						cs.SetAngularTwistLimit(s1, s2);
						cs.SetAngularSwing1Limit(s1, s2);
						cs.SetAngularSwing2Limit(s1, s2);
						cs.SetDisableCollision(true);
						NewPhysicsAsset->ConstraintSetup[i]->PostEditChange();
					}

					//NewPhysicsAsset->PostEditChange();
					//NewPhysicsAsset->MarkPackageDirty();
 
				}
			}
		}


		// see if we have skeleton set up
		// if creating skeleton, create skeleeton
		USkeleton* Skeleton = NULL;
		//Skeleton = ImportOptions->SkeletonForAnimation;
		if (Skeleton == NULL)
		{
			FString ObjectName = FString::Printf(TEXT("%s_Skeleton"), *SkeletalMesh->GetName());
			Skeleton = CreateAsset<USkeleton>(InParent->GetName(), ObjectName, true); 
			if (!Skeleton)
			{
				// same object exists, try to see if it's skeleton, if so, load
				Skeleton = LoadObject<USkeleton>(InParent, *ObjectName);

				// if not skeleton, we're done, we can't create skeleton with same name
				// @todo in the future, we'll allow them to rename
				if (!Skeleton)
				{
					AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("FbxSkeletaLMeshimport_SkeletonRecreateError", "'{0}' already exists. It fails to recreate it."), FText::FromString(ObjectName))),"FFbxErrors::SkeletalMesh_SkeletonRecreateError");
					return SkeletalMesh;
				}
			}
		}
		
		// merge bones to the selected skeleton
		if ( !Skeleton->MergeAllBonesToBoneTree( SkeletalMesh ) )
		{
			if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo,
				LOCTEXT("SkeletonFailed_BoneMerge", "FAILED TO MERGE BONES:\n\n This could happen if significant hierarchical change has been made\n - i.e. inserting bone between nodes\n Would you like to regenerate Skeleton from this mesh? \n\n ***WARNING: THIS WILL REQUIRE RECOMPRESS ALL ANIMATION DATA AND POTENTIALLY INVALIDATE***\n")))
			{
				if (Skeleton->RecreateBoneTree(SkeletalMesh))
				{
					FAssetNotifications::SkeletonNeedsToBeSaved(Skeleton);
				}
			}
		}
		else
		{
			/*
			// ask if they'd like to update their position form this mesh
			if ( ImportOptions->SkeletonForAnimation && ImportOptions->bUpdateSkeletonReferencePose ) 
			{
				Skeleton->UpdateReferencePoseFromMesh(SkeletalMesh);
				FAssetNotifications::SkeletonNeedsToBeSaved(Skeleton);
			}
			*/
		}
		if (SkeletalMesh->GetSkeleton() != Skeleton)
		{
			SkeletalMesh->SetSkeleton(Skeleton);
			SkeletalMesh->MarkPackageDirty();
		}
	}
#endif
	return SkeletalMesh;
}


UMMDExtendAsset * UPmxFactory::CreateMMDExtendFromMMDModel(
	UObject* InParent,
	USkeletalMesh* SkeletalMesh, // issue #2: fix param use skeleton mesh
	MMD4UE4::PmxMeshInfo * PmxMeshInfo
	)
{
	UMMDExtendAsset * NewMMDExtendAsset = NULL;
	check(SkeletalMesh->GetSkeleton());
	//Add UE4.9
	if (SkeletalMesh->GetSkeleton() == NULL)
	{
		return NULL;
	}


	//issue #2 : Fix MMDExtend IK Index
	const FReferenceSkeleton ReferenceSkeleton = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
	const FName& Name = FName(*SkeletalMesh->GetName());

	//MMD Extend asset

	// TBD::アセット生成関数で既存アセット時の判断ができていないと思われる。
	// 場合によってはVMDFactoryのアセット生成処理を元に再設計すること
	FString ObjectName = FString::Printf(TEXT("%s_MMDExtendAsset"), *Name.ToString());
	NewMMDExtendAsset = CreateAsset<UMMDExtendAsset>(InParent->GetName(), ObjectName, true);
	if (!NewMMDExtendAsset)
	{

		// same object exists, try to see if it's asset, if so, load
		NewMMDExtendAsset = LoadObject<UMMDExtendAsset>(InParent, *ObjectName);

		if (!NewMMDExtendAsset)
		{
			AddTokenizedErrorMessage(
				FTokenizedMessage::Create(
				EMessageSeverity::Warning,
				FText::Format(LOCTEXT("CouldNotCreateMMDExtendAsset",
				"Could not create MMD Extend Asset ('{0}') for '{1}'"),
				FText::FromString(ObjectName),
				FText::FromString(Name.ToString()))
				),"FFbxErrors::SkeletalMesh_FailedToCreatePhyscisAsset");
		}
		else
		{
			NewMMDExtendAsset->IkInfoList.Empty();
		}
	}
	
	//create asset info
	if (NewMMDExtendAsset)
	{
		if (NewMMDExtendAsset->IkInfoList.Num() > 0)
		{
			NewMMDExtendAsset->IkInfoList.Empty();
		}
		//create IK
		//mapping
		NewMMDExtendAsset->ModelName = PmxMeshInfo->modelNameJP;
		NewMMDExtendAsset->ModelComment = FText::FromString( PmxMeshInfo->modelCommentJP);
		//
		for (int boneIdx = 0; boneIdx < PmxMeshInfo->boneList.Num(); ++boneIdx)
		{
			//check IK bone 
			if (PmxMeshInfo->boneList[boneIdx].Flag_IK)
			{
				MMD4UE4::PMX_IK * tempPmxIKPtr = &PmxMeshInfo->boneList[boneIdx].IKInfo;
				FMMD_IKInfo addMMDIkInfo;

				addMMDIkInfo.LoopNum = tempPmxIKPtr->LoopNum;
				//set limit rot[rad]
				addMMDIkInfo.RotLimit = tempPmxIKPtr->RotLimit;
				// this bone
				addMMDIkInfo.IKBoneName = FName(*PmxMeshInfo->boneList[boneIdx].Name);
				//issue #2: Fix IK bone index 
				//this bone(ik-bone) index, from skeleton.
				addMMDIkInfo.IKBoneIndex = ReferenceSkeleton.FindBoneIndex(addMMDIkInfo.IKBoneName);
				//ik target 
				addMMDIkInfo.TargetBoneName = FName(*PmxMeshInfo->boneList[tempPmxIKPtr->TargetBoneIndex].Name);
				//issue #2: Fix Target Bone Index 
				//target bone(ik-target bone) index, from skeleton.
				addMMDIkInfo.TargetBoneIndex = ReferenceSkeleton.FindBoneIndex(addMMDIkInfo.TargetBoneName);
				//set sub ik
				addMMDIkInfo.ikLinkList.AddZeroed(tempPmxIKPtr->LinkNum);
				for (int ikInfoID = 0; ikInfoID < tempPmxIKPtr->LinkNum; ++ikInfoID)
				{
					//link bone index
					addMMDIkInfo.ikLinkList[ikInfoID].BoneName
						= FName(*PmxMeshInfo->boneList[tempPmxIKPtr->Link[ikInfoID].BoneIndex].Name);
					//issue #2: Fix link bone index
					//link bone index from skeleton.
					addMMDIkInfo.ikLinkList[ikInfoID].BoneIndex
						= ReferenceSkeleton.FindBoneIndex(addMMDIkInfo.ikLinkList[ikInfoID].BoneName);
					//limit flag
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockFlag = tempPmxIKPtr->Link[ikInfoID].RotLockFlag;
					//min
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockMin.X = tempPmxIKPtr->Link[ikInfoID].RotLockMin[0];
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockMin.Y = tempPmxIKPtr->Link[ikInfoID].RotLockMin[1];
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockMin.Z = tempPmxIKPtr->Link[ikInfoID].RotLockMin[2];
					//max
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockMax.X = tempPmxIKPtr->Link[ikInfoID].RotLockMax[0];
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockMax.Y = tempPmxIKPtr->Link[ikInfoID].RotLockMax[1];
					addMMDIkInfo.ikLinkList[ikInfoID].RotLockMax.Z = tempPmxIKPtr->Link[ikInfoID].RotLockMax[2];
				}
				//add
				NewMMDExtendAsset->IkInfoList.Add(addMMDIkInfo);
			}
		}
		// 
		NewMMDExtendAsset->MarkPackageDirty();
	}

	return NewMMDExtendAsset;
}

#undef LOCTEXT_NAMESPACE
