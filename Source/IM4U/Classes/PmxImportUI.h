// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
// Copyright 2015 BlackMa9. All Rights Reserved.

/**
* Copyright 2010 Autodesk, Inc. All Rights Reserved.
*
* Fbx Importer UI options.
*/

#pragma once
#include "PmxImportUI.generated.h"

/** Import mesh type */
UENUM()
enum EPMXImportType
{
	/** Select Static Mesh if you'd like to import static mesh. */
	PMXIT_StaticMesh UMETA(DisplayName = "Static Mesh"),
	/** Select Skeletal Mesh if you'd like to import skeletal mesh. */
	PMXIT_SkeletalMesh UMETA(DisplayName = "Skeletal Mesh"),
	/** Select Animation if you'd like to import only animation. */
	PMXIT_Animation UMETA(DisplayName = "Animation"),
	PMXIT_MAX,
};

UCLASS(config = EditorUserSettings, AutoExpandCategories = (FTransform), HideCategories = Object, MinimalAPI)
class UPmxImportUI : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Whether or not the imported file is in OBJ format */
	//UPROPERTY()
		bool bIsObjImport;

	/** The original detected type of this import */
	//UPROPERTY()
		TEnumAsByte<enum EPMXImportType> OriginalImportType;

	/** Type of asset to import from the FBX file */
	//UPROPERTY()
		TEnumAsByte<enum EPMXImportType> MeshTypeToImport;

		UPROPERTY(EditAnywhere, config, Category = Mesh, meta = (ImportType = "SkeletalMesh" , ToolTip = "Will use model name_jp if blank."))
		FString MeshName;

	/** Use the string in "Name" field as full name of mesh. The option only works when the scene contains one mesh. */
	//UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category = Miscellaneous, meta = (OBJRestrict = "true"))
		uint32 bOverrideFullName : 1;

	/** Whether to convert scene from FBX scene. */
	//UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category = Miscellaneous, meta = (OBJRestrict = "true", ToolTip = "Convert the scene from FBX coordinate system to UE4 coordinate system"))
		uint32 bConvertScene : 1;

	/** Whether to import the incoming FBX as a skeletal object */
	//UPROPERTY(EditAnywhere, Category = Mesh, meta = (ImportType = "StaticMesh|SkeletalMesh"))
		bool bImportAsSkeletal;

	/** Whether to import the mesh. Allows animation only import when importing a skeletal mesh. */
	//UPROPERTY(EditAnywhere, config, Category = Mesh, meta = (ImportType = "SkeletalMesh"))
		bool bImportMesh;

	/** For static meshes, enabling this option will combine all meshes in the FBX into a single monolithic mesh in Unreal */
	//UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category = Mesh, meta = (ToolTip = "If enabled, combines all meshes into a single mesh", ImportType = "StaticMesh"))
		uint32 bCombineMeshes : 1;

	/** Skeleton to use for imported asset. When importing a mesh, leaving this as "None" will create a new skeleton. When importing and animation this MUST be specified to import the asset. */
	//UPROPERTY(EditAnywhere, Category = Mesh, meta = (OBJRestrict = "false"))
	class USkeleton* Skeleton;

	/** If checked, create new PhysicsAsset if it doesn't have it */
	UPROPERTY(EditAnywhere, config, Category = Mesh, meta = (OBJRestrict = "true", ImportType = "SkeletalMesh"))
		uint32 bCreatePhysicsAsset : 1;

	/** If this is set, use this PhysicsAsset. It is possible bCreatePhysicsAsset == false, and PhysicsAsset == NULL. It is possible they do not like to create anything. */
	//UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Mesh, meta = (ImportType = "SkeletalMesh", editcondition = "!bCreatePhysicsAsset"))
	class UPhysicsAsset* PhysicsAsset;

	/** True to import animations from the FBX File */
	//UPROPERTY(EditAnywhere, config, Category = Animation, meta = (ImportType = "SkeletalMesh|Animation"))
		uint32 bImportAnimations : 1;

	/** Override for the name of the animation to import **/
	//UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Animation, meta = (editcondition = "bImportAnimations", ImportType = "SkeletalMesh"))
		FString AnimationName;

	/** Enables importing of 'rigid skeletalmesh' (unskinned, hierarchy-based animation) from this FBX file, no longer shown, used behind the scenes */
	//UPROPERTY()
		uint32 bImportRigidMesh : 1;

	/** Enable this option to use default sample rate for the imported animation at 30 frames per second */
	//UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category = Animation, meta = (editcondition = "bImportAnimations", ToolTip = "If enabled, samples all animation curves to 30 FPS", ImportType = "SkeletalMesh|Animation"))
		uint32 bUseDefaultSampleRate : 1;

	/** Whether to automatically create Unreal materials for materials found in the FBX scene */
	UPROPERTY(EditAnywhere, config, Category = Material, meta = (OBJRestrict = "true"))
		uint32 bImportMaterials : 1;

	/** The option works only when option "Import UMaterial" is OFF. If "Import UMaterial" is ON, textures are always imported. */
	UPROPERTY(EditAnywhere, config, Category = Material, meta = (OBJRestrict = "true"))
		uint32 bImportTextures : 1;

	/** create Unreal materials of MaterialInst Type */
	UPROPERTY(EditAnywhere, config, Category = Material, meta = (OBJRestrict = "true", ToolTip = "If enabled, Create Material Inst and Duplicate Mat-Assets from IM4U Base Mat. "))
		uint32 bCreateMaterialInstMode : 1;

	/** create Unreal materials of Unlit Type */
	UPROPERTY(EditAnywhere, config, Category = Material, meta = (OBJRestrict = "true", ToolTip = "If CreateMaterialInstMode enabled, effective. Create Mat Shading Model is Unlit."))
		uint32 bUnlitMaterials : 1;

	/** Import data used when importing static meshes */
	//UPROPERTY(EditAnywhere, Instanced, Category = Mesh, meta = (ImportType = "StaticMesh"))
	//class UFbxStaticMeshImportData* StaticMeshImportData;

	/** Import data used when importing static meshes */
	//UPROPERTY(EditAnywhere, Instanced, Category = Mesh, meta = (ImportType = "StaticMesh"))
		class UMMDStaticMeshImportData* StaticMeshImportData;
		
	/** Import data used when importing skeletal meshes */
	//UPROPERTY(EditAnywhere, Instanced, Category = Mesh, meta = (ImportType = "SkeletalMesh"))
	//class UFbxSkeletalMeshImportData* SkeletalMeshImportData;

	/** Import data used when importing skeletal meshes */
	//UPROPERTY(EditAnywhere, Instanced, Category = Mesh, meta = (ImportType = "SkeletalMesh"))
		class UMMDSkeletalMeshImportData* SkeletalMeshImportData;

	//暫定版::上記クラスだと上手く表示できていないため必要な項目のみ抽出
	/** True to import morph target meshes from the MMD file */
	UPROPERTY(EditAnywhere, config, Category = Mesh, meta = (OBJRestrict = "true", ToolTip = "If enabled, creates Unreal morph objects for the imported meshes"))
		uint32 bImportMorphTargets : 1;

	/** Import data used when importing animations */
	//UPROPERTY(EditAnywhere, Instanced, Category = Animation, meta = (editcondition = "bImportAnimations", ImportType = "Animation"))
	//class UFbxAnimSequenceImportData* AnimSequenceImportData;

	/** Type of asset to import from the FBX file */
	//UPROPERTY(EditAnywhere, AdvancedDisplay, config, Category = Animation, meta = (editcondition = "bImportAnimations", ImportType = "SkeletalMesh|Animation"))
		bool bPreserveLocalTransform;

	/** Import data used when importing textures */
	//UPROPERTY(EditAnywhere, Instanced, Category = Material)
	//class UFbxTextureImportData* TextureImportData;

	// Begin UObject Interface
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	// End UObject Interface

	/////////////////////////

	/** Whether to automatically create Unreal materials for materials found in the FBX scene */

	//Guiにカテゴリー「Material」内部に「Import Auto Luminous Materials」を表示する例。
	//Guiに表示される表示名は「bImportAutoLuminusMaterials」のb以下の
	//変数名(Import Auto Luminous Materials)を大文字前にスペースが入る模様。
	//UPROPERTY(EditAnywhere, config, Category = MaterialAdvMMD, meta = (OBJRestrict = "true"))
		uint32 bImportAutoLuminousMaterials : 1;

	/** Skeleton to use for imported asset. When importing a mesh, leaving this as "None" will create a new skeleton. When importing and animation this MUST be specified to import the asset. */
	//UPROPERTY(EditAnywhere, Category = Animation, meta = (OBJRestrict = "false"))
		class UAnimSequence* AnimSequenceAsset;

	/** Skeleton to use for imported asset. When importing a mesh, leaving this as "None" will create a new skeleton. When importing and animation this MUST be specified to import the asset. */
	//UPROPERTY(EditAnywhere, Category = AnimationList, meta = (ImportType = "Animation"))
		TArray<class UMMDSkeletalMeshImportData*> TestArrayList;
	
	/** MMD2UE4NameTableRow to use for imported asset. When importing a Anim, leaving this as "None" will create a new skeleton. When importing and animation this MUST be specified to import the asset. */
	//UPROPERTY(EditAnywhere, Category = Animation, meta = (OBJRestrict = "false"))
		UDataTable*  MMD2UE4NameTableRow;

	/** mmd extend assset to use for calc ik . */
	//UPROPERTY(EditAnywhere, Category = Animation, meta = (OBJRestrict = "false"))
		class UMMDExtendAsset*  MmdExtendAsset;

	//struct FTableRowBase MMD2UE4NameTableRowDmmy;


	//暫定版::上記クラスだと上手く表示できていないため必要な項目のみ抽出
	/** True to import I-Aggree , I Read README for Model. */
	//UPROPERTY(EditAnywhere, Category = Mast_ReadmeForModel, meta = (OBJRestrict = "true", ToolTip = "If enabled, creates Unreal morph objects for the imported meshes"))
	//	uint32 bImportIAgree : 1;
};



