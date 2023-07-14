
// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once


#include "Engine.h"

#include "Factories/Factory.h"
#include "Factories.h"


#include "ImportUtils\SkelImport.h"
#include "AnimationUtils.h"

#include "VmdImportUI.h"

#include "VmdImporter.h"

#include "MMD2UE4NameTableRow.h"
#include "MMDExtendAsset.h"



#include "VmdFactory.generated.h"

// Forward declaration
class UInterpGroupInst;

DECLARE_LOG_CATEGORY_EXTERN(LogMMD4UE4_VMDFactory, Log, All)

UCLASS()
class IM4U_API UVmdFactory : public UFactory 
{
	GENERATED_UCLASS_BODY()

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

	/**********************
	* Create AnimSequence from VMD data.(新規作成用親関数)
	***********************/
	UAnimSequence * ImportAnimations(
		USkeleton* Skeleton,
		USkeletalMesh* SkeletalMesh,
		UObject* Outer,
		const FString& Name, 
		//UFbxAnimSequenceImportData* TemplateImportData, 
		//TArray<FbxNode*>& NodeArray
		UDataTable* ReNameTable,
		UMMDExtendAsset* mmdExtend,
		MMD4UE4::VmdMotionInfo* vmdMotionInfo
		);
	//////////////
	class UVmdImportUI* ImportUI;
	/**********
	* MMD向けベジェ曲線の算出処理
	***********/
	float interpolateBezier(float x1, float y1, float  x2, float y2, float x);
	/*******************
	* 既存AnimSequのアセットにVMDの表情データを追加する処理
	* MMD4Mecanimuとの総合利用向けテスト機能
	**********************/
	UAnimSequence * AddtionalMorphCurveImportToAnimations(
		USkeletalMesh* SkeletalMesh,
		UAnimSequence* exsistAnimSequ,
		UDataTable* ReNameTable,
		MMD4UE4::VmdMotionInfo* vmdMotionInfo
		);
	/*******************
	* Import Morph Target AnimCurve
	* VMDファイルのデータからMorphtargetのFloatCurveをAnimSeqに取り込む
	**********************/
	bool ImportMorphCurveToAnimSequence(
		UAnimSequence* DestSeq,
		USkeleton* Skeleton,
		USkeletalMesh* SkeletalMesh,
		UDataTable* ReNameTable,
		MMD4UE4::VmdMotionInfo* vmdMotionInfo
		);
	/*******************
	* Import VMD Animation
	* VMDファイルのデータからモーションデータをAnimSeqに取り込む
	**********************/
	bool ImportVMDToAnimSequence(
		UAnimSequence* DestSeq,
		USkeleton* Skeleton,
		UDataTable* ReNameTable,
		UMMDExtendAsset* mmdExtend,
		MMD4UE4::VmdMotionInfo* vmdMotionInfo
		);

	/*****************
	* MMD側の名称からTableRowのUE側名称を検索し取得する
	* Return :T is Found
	* @param :ue4Name is Found Row Name
	****************/
	bool FindTableRowMMD2UEName(
		UDataTable* ReNameTable,
		FName mmdName,
		FName * ue4Name
		);
	/*****************
	* Bone名称からRefSkeltonで一致するBoneIndexを検索し取得する
	* Return :index, -1 is not found
	* @param :TargetName is Target Bone Name
	****************/
	int32 FindRefBoneInfoIndexFromBoneName(
		const FReferenceSkeleton & RefSkelton,
		const FName & TargetName
		);
	/*****************
	* 現在のキーにおける指定BoneのGlb座標を再帰的に算出する
	* Return :trncform
	* @param :TargetName is Target Bone Name
	****************/
	FTransform CalcGlbTransformFromBoneIndex(
		UAnimSequence* DestSeq,
		USkeleton* Skeleton,
		int32 BoneIndex,
		int32 keyIndex
		);
	////////////////////////////////////////



	void ImportAnimatedProperty(
		float* Value,
		const TCHAR* ValueName,
		UInterpGroupInst* MatineeGroup,
		const float FbxValue,
		//FbxProperty InProperty,
		bool bImportFOV,
		MMD4UE4::VmdCameraTrackList * Camera
		);

	/**
	* Imports a FBX scene node into a Matinee actor group.
	*/
	float ImportMatineeActor(
		MMD4UE4::VmdCameraTrackList* CameraNode,
		//FbxNode* Node,
		//上記に該当するデータを調査すること
		UInterpGroupInst* MatineeGroup
		);
	//

	//この関数の戻り値はMMDのカーブ特性に合わせて固定値を変えさせるように修正する
	EInterpCurveMode GetUnrealInterpMode(
		//FbxAnimCurveKey FbxKey
		);

	//この関数が必要か不明。MatineeAnimatedがどの部分に該当するかについて調査が必要。
	void ImportMatineeAnimated(
		MMD4UE4::VmdCameraTrackList * VmdCurve,
		//FbxAnimCurve* FbxCurve,
		FInterpCurveVector& Curve,
		int32 CurveIndex,
		bool bNegative,
		//FbxAnimCurve* RealCurve,
		float DefaultVal
		);
	//base flame rate [ms]
	float baseFrameRate;
};
