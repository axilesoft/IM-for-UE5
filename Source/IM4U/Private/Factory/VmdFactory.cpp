// Copyright 2015 BlackMa9. All Rights Reserved.

#include "Factory/VmdFactory.h"
#include "../IM4UPrivatePCH.h"

#include "VmdImporter.h"

#include "CoreMinimal.h"
#include "ImportUtils/SkelImport.h"
#include "AnimationUtils.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "VmdImportUI.h"


#include "Factory/VmdImportOption.h"

#define LOCTEXT_NAMESPACE "VMDImportFactory"
#define USE_RM 0

DEFINE_LOG_CATEGORY(LogMMD4UE4_VMDFactory)

static TMap<FName, FName> NameMap;
#define ADD_NAME_MAP( x , y ) NameMap.Add((y),(x))
void initMmdNameMap() {


#if 0
	if (NameMap.Num() == 0)
	{
		ADD_NAME_MAP(L"操作中心", L"op_center");
		ADD_NAME_MAP(L"全ての親", L"all_parent");
		ADD_NAME_MAP(L"センター", L"center");
		ADD_NAME_MAP(L"センター2", L"center2");
		ADD_NAME_MAP(L"グルーブ", L"groove");
		ADD_NAME_MAP(L"グルーブ2", L"groove2");
		ADD_NAME_MAP(L"腰", L"waist");
		ADD_NAME_MAP(L"下半身", L"lowerBody");
		ADD_NAME_MAP(L"上半身", L"upperBody");
		ADD_NAME_MAP(L"上半身", L"upperBody2");
		ADD_NAME_MAP(L"首", L"neck");
		ADD_NAME_MAP(L"頭", L"head");
		ADD_NAME_MAP(L"左目", L"eyeL");
		ADD_NAME_MAP(L"右目", L"eyeR");
		
		ADD_NAME_MAP(L"左肩", L"shoulderL");
		ADD_NAME_MAP(L"左腕", L"armL");
		ADD_NAME_MAP(L"左ひじ", L"elbowL");
		ADD_NAME_MAP(L"左手首", L"wristL");
		ADD_NAME_MAP(L"左親指０", L"thumb0L");
		ADD_NAME_MAP(L"左親指１", L"thumb1L");
		ADD_NAME_MAP(L"左親指２", L"thumb2L");
		ADD_NAME_MAP(L"左人指０", L"fore0L");
		ADD_NAME_MAP(L"左人指１", L"fore1L");
		ADD_NAME_MAP(L"左人指２", L"fore2L");
		ADD_NAME_MAP(L"左中指０", L"middle0L");
		ADD_NAME_MAP(L"左中指１", L"middle1L");
		ADD_NAME_MAP(L"左中指２", L"middle2L");
		ADD_NAME_MAP(L"左薬指０", L"third0L");
		ADD_NAME_MAP(L"左薬指１", L"third1L");
		ADD_NAME_MAP(L"左薬指２", L"third2L");
		ADD_NAME_MAP(L"左小指０", L"little0L");
		ADD_NAME_MAP(L"左小指１", L"little1L");
		ADD_NAME_MAP(L"左小指２", L"little2L");
		ADD_NAME_MAP(L"左足", L"legL");
		ADD_NAME_MAP(L"左ひざ", L"kneeL");
		ADD_NAME_MAP(L"左足首", L"ankleL");
		
		ADD_NAME_MAP(L"右肩", L"shoulderR");
		ADD_NAME_MAP(L"右腕", L"armR");
		ADD_NAME_MAP(L"右ひじ", L"elbowR");
		ADD_NAME_MAP(L"右手首", L"wristR");
		ADD_NAME_MAP(L"右親指０", L"thumb0R");
		ADD_NAME_MAP(L"右親指１", L"thumb1R");
		ADD_NAME_MAP(L"右親指２", L"thumb2R");
		ADD_NAME_MAP(L"右人指０", L"fore0R");
		ADD_NAME_MAP(L"右人指１", L"fore1R");
		ADD_NAME_MAP(L"右人指２", L"fore2R");
		ADD_NAME_MAP(L"右中指０", L"middle0R");
		ADD_NAME_MAP(L"右中指１", L"middle1R");
		ADD_NAME_MAP(L"右中指２", L"middle2R");
		ADD_NAME_MAP(L"右薬指０", L"third0R");
		ADD_NAME_MAP(L"右薬指１", L"third1R");
		ADD_NAME_MAP(L"右薬指２", L"third2R");
		ADD_NAME_MAP(L"右小指０", L"little0R");
		ADD_NAME_MAP(L"右小指１", L"little1R");
		ADD_NAME_MAP(L"右小指２", L"little2R");
		ADD_NAME_MAP(L"右足", L"legR");
		ADD_NAME_MAP(L"右ひざ", L"kneeR");
		ADD_NAME_MAP(L"右足首", L"ankleR");

		ADD_NAME_MAP(L"両目", L"eyes");
		ADD_NAME_MAP(L"左足ＩＫ", L"ikLegL");
		ADD_NAME_MAP(L"右足ＩＫ", L"ikLegR");
		ADD_NAME_MAP(L"左つま先ＩＫ", L"ikToeL");
		ADD_NAME_MAP(L"右つま先ＩＫ", L"ikToeR");

	}
#endif
}
/////////////////////////////////////////////////////////
//prototype ::from dxlib 
// Ｘ軸を中心とした回転行列を作成する
void CreateRotationXMatrix(FMatrix *Out, float Angle);
// 回転成分だけの行列の積を求める( ３×３以外の部分には値も代入しない )
void MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(FMatrix *Out, FMatrix *In1, FMatrix *In2);
// 角度制限を判定する共通関数 (subIndexJdgの判定は割りと不明…)
void CheckLimitAngle(
	const FVector& RotMin,
	const FVector& RotMax,
	FVector * outAngle, //target angle ( in and out param)
	bool subIndexJdg //(ik link index < ik loop temp):: linkBoneIndex < ikt
	);
///////////////////////////////////////////////////////

UVmdFactory::UVmdFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	SupportedClass = NULL;
	//SupportedClass = UPmxFactory::StaticClass();
	Formats.Empty();

	Formats.Add(TEXT("vmd;vmd animations"));

	bCreateNew = false;
	bText = false;
	bEditorImport = true;

	initMmdNameMap();

}

void UVmdFactory::PostInitProperties()
{
	Super::PostInitProperties();

	ImportUI = NewObject<UVmdImportUI>(this, NAME_None, RF_NoFlags);
}

bool UVmdFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UVmdFactory::StaticClass());
}

UClass* UVmdFactory::ResolveSupportedClass()
{
	return UVmdFactory::StaticClass();
}

UObject* UVmdFactory::FactoryCreateBinary
(
	UClass*				Class,
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
	MMD4UE4::VmdMotionInfo vmdMotionInfo;

	if (vmdMotionInfo.VMDLoaderBinary(Buffer, BufferEnd) == false)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Error,
			TEXT("VMD Import Cancel:: vmd data load faile."));
		return NULL;
	}

	/////////////////////////////////////////
	UAnimSequence* LastCreatedAnim = NULL;
	USkeleton* Skeleton = NULL;
	USkeletalMesh* SkeletalMesh = NULL;
	VMDImportOptions* ImportOptions = NULL;
	//////////////////////////////////////

	/***************************************
	* IM4U 暫定版 仕様通知メッセージ
	****************************************/
	if (false)
	{
		//モデル読み込み後の警告文表示：コメント欄
		FText TitleStr = FText::Format(LOCTEXT("ImportReadMe_Generic_Dbg", "{0} 制限事項"), FText::FromString("IM4U Plugin"));
		const FText MessageDbg
			= FText(LOCTEXT("ImportReadMe_Generic_Dbg_Comment",
"現時点で有効なパラメータは、以下です\n\n\
::Skeleton Asset(必須::Animation関連付け先)\n\
::SkeletalMesh Asset(任意::Animation関連付け先、MorphTarget. NULLならばMorphTargetはSkipします。)\n\
::Animation Asset(NULL以外で既存AssetにMorphのみ追加する処理を実行。NULLでBoneとMorph含む新規Asset作成)\n\
::DataTable(MMD2UE4Name) Asset(任意：NULL以外で読み込み時にBoneとMorphNameをMMD=UE4で読み替えてImportを実行する。事前にCSV形式でImportか新規作成しておく必要あり。)\n\
::MmdExtendAsse(任意：NULL以外でVMDからAnimSeqアセット生成時にExtendからIK情報を参照し計算する際に使用。事前にモデルインポートか手動にてアセット生成しておくこと。)\n\
\n\
\n\
注意：新規Asset生成はIKなど未対応の為非推奨。追加Morphのみ対応。"
			)
			);
		FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
	}
	/***************************************
	* VMD取り込み時の警告表示
	****************************************/
	if (false)
	{
		//モデル読み込み後の警告文表示：コメント欄
		FText TitleStr = FText(LOCTEXT("ImportVMD_TargetModelInfo", "警告[ImportVMD_TargetModelInfo]"));
		const FText MessageDbg
			= FText::Format(LOCTEXT("ImportVMD_TargetModelInfo_Comment",
			"注意：モーションデータ取り込み情報：：\n\
\n\
本VMDは、「{0}」用に作成されたファイルです。\n\
\n\
モデル向けモーションの場合、同じボーン名のデータのみ取り込まれます。\n\
モデル側のボーン名と異なる名称の同一ボーンが含まれる場合、\n\
事前に変換テーブル(MMD2UE4NameTableRow)を作成し、\n\
InportOption画面にて指定することで取り込むことが可能です。"
			)
			, FText::FromString(vmdMotionInfo.ModelName)
			);
		FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
	}
	/////////////////////////////////////
	// factory animation asset from vmd data.
	////////////////////////////////////
	if (vmdMotionInfo.keyCameraList.Num() == 0)
	{
		//カメラアニメーションでない場合
		FVmdImporter* VmdImporter = FVmdImporter::GetInstance();

		EVMDImportType ForcedImportType = VMDIT_Animation;
		bool bOperationCanceled = false;
		bool bIsPmxFormat = true;
		// show Import Option Slate
		bool bImportAll = false;
		ImportUI->bIsObjImport = false;//anim mode
		ImportUI->OriginalImportType = EVMDImportType::VMDIT_Animation;
		ImportOptions
			= GetVMDImportOptions(
				VmdImporter,
				ImportUI,
				true,//bShowImportDialog, 
				InParent->GetPathName(),
				bOperationCanceled,
				bImportAll,
				ImportUI->bIsObjImport,//bIsPmxFormat,
				bIsPmxFormat,
				ForcedImportType
				);
		/* 一度目の判定 */
		if (ImportOptions)
		{
			Skeleton = ImportUI->Skeleton;
			SkeletalMesh = ImportUI->SkeletonMesh;
			/* 最低限のパラメータ設定チェック */
			if (!Skeleton && SkeletalMesh)
				Skeleton = SkeletalMesh->GetSkeleton();
			if ( (!Skeleton) 
				||  (SkeletalMesh) && (Skeleton != SkeletalMesh->GetSkeleton())
				)
			{

				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					TEXT("[ImportAnimations]::Parameter check for Import option!"));

				{
					//モデル読み込み後の警告文表示：コメント欄
					FText TitleStr = FText(LOCTEXT("ImportVMD_OptionWarn_CheckPh1", "警告[ImportVMD_TargetModelInfo]"));
					const FText MessageDbg
						= FText::Format(LOCTEXT("ImportVMD_OptionWarn_CheckPh1_Comment",
							"注意：Parameter check for Import option ：：\n\
\n\
[Mandatory](必須)\n\
- Skeleton Asset : you select target skeleton .\n\
	if NULL , import error.\n\
[Optional](任意)\n\
- SkeletalMesh Asset : you select target skeletal mesh. \n\
- However , SkeltalMesh include skeleton. (ただし、必ずメッシュは同じスケルトンを選ぶこと)\n\
	if NULL , skip import morph curve.(モーフが取り込まれません)\n\
\n\
Retry ImportOption!"
						)
							, FText::FromString(vmdMotionInfo.ModelName)
						);
					FMessageDialog::Open(EAppMsgType::Ok, MessageDbg, &TitleStr);
				}
				/* もう一回させる*/
				ImportOptions
					= GetVMDImportOptions(
						VmdImporter,
						ImportUI,
						true,//bShowImportDialog, 
						InParent->GetPathName(),
						bOperationCanceled,
						bImportAll,
						ImportUI->bIsObjImport,//bIsPmxFormat,
						bIsPmxFormat,
						ForcedImportType
					);
			}
		}
		if (ImportOptions)
		{
			if (!Skeleton) Skeleton = ImportUI->Skeleton;
			if (!SkeletalMesh) SkeletalMesh = ImportUI->SkeletonMesh;
			bool preParamChk = true;/*ParameterチェックOK有無*/
			/*包含関係チェック*/
			if (SkeletalMesh)
			{
				if (Skeleton != SkeletalMesh->GetSkeleton())
				{
					//TBD::ERR case
					{
						UE_LOG(LogMMD4UE4_VMDFactory, Error,
							TEXT("ImportAnimations : Skeleton not equrl skeletalmesh->skelton ...")
						);
					}
					preParamChk = false;
				}
			}
			if (preParamChk)
			{

				////////////////////////////////////
				if (!ImportOptions->AnimSequenceAsset)
				{
					//create AnimSequence Asset from VMD
					LastCreatedAnim = ImportAnimations(
						Skeleton,
						SkeletalMesh,
						InParent,
						Name.ToString(),
						ImportUI->MMD2UE4NameTableRow,
						ImportUI->MmdExtendAsset,
						&vmdMotionInfo
					);
				}
				else
				{
					//TBD::OptionでAinimSeqが選択されていない場合、終了
					// add morph curve only to exist ainimation
					LastCreatedAnim = AddtionalMorphCurveImportToAnimations(
						SkeletalMesh,
						ImportOptions->AnimSequenceAsset,//UAnimSequence* exsistAnimSequ,
						ImportUI->MMD2UE4NameTableRow,
						&vmdMotionInfo
					);
				}
			}
			else
			{
				//TBD::ERR case
				{
					UE_LOG(LogMMD4UE4_VMDFactory, Error,
						TEXT("ImportAnimations : preParamChk false. import ERROR !!!! ...")
					);
				}
			}
		}
		else
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Warning, 
				TEXT("VMD Import Cancel"));
		}
	}
	else
	{
		//カメラアニメーションのインポート処理
		//今後実装予定？
		UE_LOG(LogMMD4UE4_VMDFactory, Warning,
			TEXT("VMD Import Cancel::Camera root... not impl"));

		LastCreatedAnim = NULL;
		//未実装なのでコメントアウト
#ifdef IM4U_FACTORY_MATINEEACTOR_VMD
		////////////////////////
		// Import Optionを設定するslateに関しては必要ない認識。
		//

		// アセットの生成
		//AMatineeActor* InMatineeActor = NULL;
		//ここにアセットの基本生成処理もしくは既存アセットの再利用処理を実装すること。

		//targetのアセットに対しカメラアニメーションをインポートさせる
		if (ImportMatineeSequence(
			InMatineeActor,
			vmdMotionInfo
			) == false)
		{
			//import error
		}
#endif 
	}
	return LastCreatedAnim;
};

UAnimSequence * UVmdFactory::ImportAnimations(
	USkeleton* Skeleton,
	USkeletalMesh* SkeletalMesh,
	UObject* Outer,
	const FString& Name,
	//UFbxAnimSequenceImportData* TemplateImportData, 
	//TArray<FbxNode*>& NodeArray
	UDataTable* ReNameTable,
	UMMDExtendAsset* mmdExtend,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
	)
{
	UAnimSequence* LastCreatedAnim = NULL;


	if (!Skeleton)
		Skeleton = SkeletalMesh->GetSkeleton();
	// we need skeleton to create animsequence
	if (Skeleton == NULL)
	{
		//TBD::ERR case
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Error,
				TEXT("ImportAnimations : args Skeleton is null ...")
			);
		}
		return NULL;
	}

	{
		FString SequenceName = Name;

		//if (ValidTakeCount > 1)
		{
			SequenceName += "_";
			//SequenceName += ANSI_TO_TCHAR(CurAnimStack->GetName());
			SequenceName += Skeleton->GetName();
		}

		// See if this sequence already exists.
		SequenceName = ObjectTools::SanitizeObjectName(SequenceName);

		FString 	ParentPath = FString::Printf(TEXT("%s/%s"), *FPackageName::GetLongPackagePath(*Outer->GetName()), *SequenceName);
		UObject* 	ParentPackage = CreatePackage( *ParentPath);
		UObject* Object = LoadObject<UObject>(ParentPackage, *SequenceName, NULL, LOAD_None, NULL);
		UAnimSequence * DestSeq = Cast<UAnimSequence>(Object);
		// if object with same name exists, warn user
		if (Object && !DestSeq)
		{
			//AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("Error_AssetExist", "Asset with same name exists. Can't overwrite another asset")), FFbxErrors::Generic_SameNameAssetExists);
			//continue; // Move on to next sequence..
			return LastCreatedAnim;
		}

		// If not, create new one now.
		if (!DestSeq)
		{
			DestSeq = NewObject<UAnimSequence>( ParentPackage, *SequenceName, RF_Public | RF_Standalone);

			// Notify the asset registry
			FAssetRegistryModule::AssetCreated(DestSeq);
		}
		else
		{
			DestSeq->ResetAnimation();
		}

		DestSeq->SetSkeleton(Skeleton);
		/*
		// since to know full path, reimport will need to do same
		UFbxAnimSequenceImportData* ImportData = UFbxAnimSequenceImportData::GetImportDataForAnimSequence(DestSeq, TemplateImportData);
		ImportData->SourceFilePath = FReimportManager::SanitizeImportFilename(UFactory::CurrentFilename, DestSeq);
		ImportData->SourceFileTimestamp = IFileManager::Get().GetTimeStamp(*UFactory::CurrentFilename).ToString();
		ImportData->bDirty = false;

		ImportAnimation(Skeleton, DestSeq, Name, SortedLinks, NodeArray, CurAnimStack, ResampleRate, AnimTimeSpan);
		*/
		LastCreatedAnim = DestSeq;
	}
	///////////////////////////////////
	// Create RawCurve -> Track Curve Key
	//////////////////////
	if (LastCreatedAnim)
	{
		bool importSuccessFlag = true;
		/* vmd animation regist*/
		if (!ImportVMDToAnimSequence(LastCreatedAnim, Skeleton, ReNameTable, mmdExtend, vmdMotionInfo))
		{
			//TBD::ERR case
			check(false);
			importSuccessFlag = false;
		}
		/* morph animation regist*/
		if (!ImportMorphCurveToAnimSequence(LastCreatedAnim, Skeleton, SkeletalMesh, ReNameTable,vmdMotionInfo))
		{
			//TBD::ERR case
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Error,
					TEXT("ImportMorphCurveToAnimSequence is false root...")
					);
			}
			//check(false);
			importSuccessFlag = false;
		}

		/*Import正常時にPreviewMeshを更新する*/
		if ( (importSuccessFlag) && (SkeletalMesh) )
		{
			LastCreatedAnim->SetPreviewMesh(SkeletalMesh);
			UE_LOG(LogMMD4UE4_VMDFactory, Log,
				TEXT("[ImportAnimations] Set PreviewMesh Pointer.")
			);
		}
	}
	//LastCreatedAnim->BoneCompressionSettings

	/////////////////////////////////////////
	// end process?
	////////////////////////////////////////
	if (LastCreatedAnim)
	{
		/***********************/
		// refresh TrackToskeletonMapIndex
		//LastCreatedAnim->RefreshTrackMapFromAnimTrackNames();
		if (false)
		{
			//LastCreatedAnim->BakeTrackCurvesToRawAnimation();
		}
		else if (SkeletalMesh)
		{
			// otherwise just compress
			//LastCreatedAnim->PostProcessSequence();
			auto& adc = LastCreatedAnim->GetController();
			adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"));

			//Controller.SetPlayLength(AbcFile->GetImportLength());
			//Controller.SetFrameRate(FFrameRate(AbcFile->GetFramerate(), 1));

			adc.UpdateCurveNamesFromSkeleton(Skeleton, ERawCurveTrackTypes::RCT_Float);
			adc.NotifyPopulated();

			adc.CloseBracket();

				// mark package as dirty
				MarkPackageDirty();
			SkeletalMesh->MarkPackageDirty();



			LastCreatedAnim->PostEditChange();
			LastCreatedAnim->SetPreviewMesh(SkeletalMesh);
			LastCreatedAnim->MarkPackageDirty();

			Skeleton->SetPreviewMesh(SkeletalMesh);
			Skeleton->PostEditChange();
		}
	}

	return LastCreatedAnim;
}

/******************************************************************************
* Start
* copy from: http://d.hatena.ne.jp/edvakf/touch/20111016/1318716097
* x1~y2 : 0 <= xy <= 1 :bezier points
* x : 0<= x <= 1 : frame rate
******************************************************************************/
float UVmdFactory::interpolateBezier(float x1, float y1, float  x2, float y2, float x) {
	float t = 0.5, s = 0.5;
	for (int i = 0; i < 15; i++) {
		float ft = (3 * s * s * t * x1) + (3 * s * t * t * x2) + (t * t * t) - x;
		if (ft == 0) break; // Math.abs(ft) < 0.00001 でもいいかも
		if (FGenericPlatformMath::Abs(ft) < 0.0001) break;
		if (ft > 0)
			t -= 1.0 / (float)(4 << i);
		else // ft < 0
			t += 1.0 / (float)(4 << i);
		s = 1 - t;
	}
	return (3 * s * s * t * y1) + (3 * s * t * t * y2) + (t * t * t);
}
/** End **********************************************************************/


/*******************
* 既存AnimSequのアセットにVMDの表情データを追加する処理
* MMD4Mecanimuとの総合利用向けテスト機能
**********************/
UAnimSequence * UVmdFactory::AddtionalMorphCurveImportToAnimations(
	USkeletalMesh* SkeletalMesh,
	UAnimSequence* exsistAnimSequ,
	UDataTable* ReNameTable,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
	)
{
	USkeleton* Skeleton = NULL;
	// we need skeleton to create animsequence
	if (exsistAnimSequ == NULL)
	{
		return NULL;
	}
	{
		//TDB::if exsite assets need fucn?
		//exsistAnimSequ->RecycleAnimSequence();

		Skeleton = exsistAnimSequ->GetSkeleton();
	}
	///////////////////////////////////
	// Create RawCurve -> Track Curve Key
	//////////////////////
	if (exsistAnimSequ)
	{
		//exsistAnimSequ->NumFrames = vmdMotionInfo->maxFrame;
		//exsistAnimSequ->SequenceLength = FGenericPlatformMath::Max<float>(1.0f / 30.0f*(float)exsistAnimSequ->NumFrames, MINIMUM_ANIMATION_LENGTH);
		/////////////////////////////////
		if (!ImportMorphCurveToAnimSequence(
			exsistAnimSequ,
			Skeleton, 
			SkeletalMesh,
			ReNameTable, 
			vmdMotionInfo)
			)
		{
			//TBD::ERR case
			check(false);
		}
	}

	/////////////////////////////////////////
	// end process?
	////////////////////////////////////////
	if (exsistAnimSequ)
	{
		bool existAsset = true;
		/***********************/
		// refresh TrackToskeletonMapIndex
		//exsistAnimSequ->RefreshTrackMapFromAnimTrackNames();
		if (existAsset)
		{
			//exsistAnimSequ->BakeTrackCurvesToRawAnimation();
		}
		else
		{
			// otherwise just compress
			//exsistAnimSequ->PostProcessSequence();
			
			auto& adc = exsistAnimSequ->GetController();
			adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"));

			//Controller.SetPlayLength(AbcFile->GetImportLength());
			//Controller.SetFrameRate(FFrameRate(AbcFile->GetFramerate(), 1));

			adc.UpdateCurveNamesFromSkeleton(Skeleton, ERawCurveTrackTypes::RCT_Float);
			adc.NotifyPopulated();

			adc.CloseBracket();

			// mark package as dirty
			MarkPackageDirty();
			SkeletalMesh->MarkPackageDirty();



			exsistAnimSequ->PostEditChange();
			exsistAnimSequ->SetPreviewMesh(SkeletalMesh);
			exsistAnimSequ->MarkPackageDirty();

			Skeleton->SetPreviewMesh(SkeletalMesh);
			Skeleton->PostEditChange();
		}
	}

	return exsistAnimSequ;
}
/*******************
* Import Morph Target AnimCurve
* VMDファイルのデータからMorphtargetのFloatCurveをAnimSeqに取り込む
**********************/
bool UVmdFactory::ImportMorphCurveToAnimSequence(
	UAnimSequence* DestSeq,
	USkeleton* Skeleton,
	USkeletalMesh* SkeletalMesh,
	UDataTable* ReNameTable,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
	)
{
	if (!DestSeq  || !vmdMotionInfo)
	{
		//TBD:: ERR in Param...
		return false;
	}

	auto& adc = DestSeq->GetController();

	const bool bShouldTransact = true;
	adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"), bShouldTransact);

	//USkeletalMesh * mesh = Skeleton->GetAssetPreviewMesh(DestSeq);// GetPreviewMesh();
	USkeletalMesh * mesh = SkeletalMesh;
	if (!mesh)
	{
		//このルートに入る条件がSkeleton Asset生成後一度もアセットを開いていない場合、
		// NULLの模様。この関数を使うよりも別の手段を考えた方が良さそう…。要調査枠。
		//TDB::ERR.  previewMesh is Null
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Error,
				TEXT("ImportMorphCurveToAnimSequence GetAssetPreviewMesh Not Found...")
				);
		}
		return false;
	}
	/* morph animation regist*/
	for (int i = 0; i < vmdMotionInfo->keyFaceList.Num(); ++i)
	{
		MMD4UE4::VmdFaceTrackList * vmdFaceTrackPtr = &vmdMotionInfo->keyFaceList[i];
		/********************************************/
		//original
		FName Name = *vmdFaceTrackPtr->TrackName;

#if 0	/* under ~UE4.10*/
		FSmartNameMapping* NameMapping 
			//= Skeleton->SmartNames.GetContainer(USkeleton::AnimCurveMappingName); 
#else	/* UE4.11~ over */
		const FSmartNameMapping* NameMapping
			//= const_cast<FSmartNameMapping*>(Skeleton->GetSmartNameContainer(USkeleton::AnimCurveMappingName));//UE4.11~
			= Skeleton->GetSmartNameContainer(USkeleton::AnimCurveMappingName);//UE4.11~
#endif
		/**********************************/
		//self
		if (mesh != NULL)
		{
			UMorphTarget * morphTargetPtr = mesh->FindMorphTarget(Name);
			if (!morphTargetPtr)
			{
				//TDB::ERR. not found Morph Target(Name) in mesh
				{
					UE_LOG(LogMMD4UE4_VMDFactory, Warning,
						TEXT("ImportMorphCurveToAnimSequence Target Morph Not Found...Search[%s]VMD-Org[%s]"),
						*Name.ToString(), *vmdFaceTrackPtr->TrackName);
				}
				continue;
			}
		}
		/*********************************/
		// Add or retrieve curve
		if (!NameMapping->Exists(Name))
		{
			// mark skeleton dirty
			Skeleton->Modify();
		}

#if 0

		FSmartName NewName;
		Skeleton->AddSmartNameAndModify(USkeleton::AnimCurveMappingName, Name, NewName);

		// FloatCurve for Morph Target 
		int CurveFlags = AACF_DriveMorphTarget_DEPRECATED;

		FFloatCurve * CurveToImport = (FFloatCurve*)DestSeq->GetCurveData().GetCurveData(NewName.UID);
		if (CurveToImport == NULL)
		{
			if ( ((FRawCurveTracks*)(&DestSeq->GetCurveData()))->AddCurveData(NewName))
			{
				CurveToImport
					= (FFloatCurve*)(DestSeq->GetCurveData().GetCurveData(NewName.UID));
				//CurveToImport->Name = NewName;
			}
			else
			{
				// this should not happen, we already checked before adding
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					TEXT("VMD Import: Critical error: no memory?"));
			}
		}
		else
		{
			CurveToImport->FloatCurve.Reset();
			// if existing add these curve flags. 
			CurveToImport->SetCurveTypeFlags(CurveFlags | CurveToImport->GetCurveTypeFlags());
		}
		
		/**********************************************/
		MMD4UE4::VMD_FACE_KEY * faceKeyPtr = NULL;
		for (int s = 0; s < vmdFaceTrackPtr->keyList.Num(); ++s)
		{
			check(vmdFaceTrackPtr->sortIndexList[s] < vmdFaceTrackPtr->keyList.Num());
			faceKeyPtr = &vmdFaceTrackPtr->keyList[vmdFaceTrackPtr->sortIndexList[s]];
			check(faceKeyPtr);
			/********************************************/
			float timeCurve = faceKeyPtr->Frame / 30.0f;
			if (timeCurve > DestSeq->GetPlayLength())
			{
				//this key frame(time) more than Target SeqLength ... 
				break;
			}
			CurveToImport->FloatCurve.AddKey(timeCurve, faceKeyPtr->Factor, true);
			/********************************************/
		}

		// update last observed name. If not, sometimes it adds new UID while fixing up that will confuse Compressed Raw Data
		//const FSmartNameMapping* Mapping = Skeleton->GetSmartNameContainer(USkeleton::AnimCurveMappingName);
		//DestSeq->GetCurveData().RefreshName(Mapping);

		//DestSeq->MarkRawDataAsModified();
		/***********************************************************************************/
		// Trace Log ( for debug message , compleat import morph of this track )
		if (true)
		{
			UE_LOG(LogMMD4UE4_VMDFactory, Log,
				TEXT("ImportMorphCurveToAnimSequence Target Morph compleat...NameSearch[%s]VMD-Org[%s], KeyListNum[%d]"),
				*Name.ToString(), *vmdFaceTrackPtr->TrackName, vmdFaceTrackPtr->keyList.Num() );
		}
		/***********************************************************************************/

#else
		if (vmdFaceTrackPtr->keyList.Num() > 1) {
			FSmartName NewName;
			Skeleton->AddSmartNameAndModify(USkeleton::AnimCurveMappingName, Name, NewName);

			FAnimationCurveIdentifier CurveId(NewName, ERawCurveTrackTypes::RCT_Float);
			adc.AddCurve(CurveId);

			const FFloatCurve* NewCurve = DestSeq->GetDataModel()->FindFloatCurve(CurveId);
			ensure(NewCurve);
			MMD4UE4::VMD_FACE_KEY* faceKeyPtr = NULL;
			FRichCurve RichCurve;
			for (int s = 0; s < vmdFaceTrackPtr->keyList.Num(); ++s)
			{
				check(vmdFaceTrackPtr->sortIndexList[s] < vmdFaceTrackPtr->keyList.Num());
				faceKeyPtr = &vmdFaceTrackPtr->keyList[vmdFaceTrackPtr->sortIndexList[s]];
				check(faceKeyPtr);
				/********************************************/
				float timeCurve = faceKeyPtr->Frame / 30.0f;
				if (timeCurve > DestSeq->GetPlayLength())
				{
					//this key frame(time) more than Target SeqLength ... 
					break;
				}

				const float CurveValue = faceKeyPtr->Factor;
				const float TimeValue = timeCurve;

				FKeyHandle NewKeyHandle = RichCurve.AddKey(TimeValue, CurveValue, false);

				ERichCurveInterpMode NewInterpMode = RCIM_Linear;
				ERichCurveTangentMode NewTangentMode = RCTM_Auto;
				ERichCurveTangentWeightMode NewTangentWeightMode = RCTWM_WeightedNone;

				RichCurve.SetKeyInterpMode(NewKeyHandle, NewInterpMode);
				RichCurve.SetKeyTangentMode(NewKeyHandle, NewTangentMode);
				RichCurve.SetKeyTangentWeightMode(NewKeyHandle, NewTangentWeightMode);
			}
			adc.SetCurveKeys(CurveId, RichCurve.GetConstRefOfKeys());
		}
#endif
	}


	adc.NotifyPopulated();
	adc.CloseBracket();
	return true;
}


/*******************
* Import VMD Animation
* VMDファイルのデータからモーションデータをAnimSeqに取り込む
**********************/
bool UVmdFactory::ImportVMDToAnimSequence(
	UAnimSequence* DestSeq,
	USkeleton* Skeleton,
	UDataTable* ReNameTable,
	UMMDExtendAsset* mmdExtend,
	MMD4UE4::VmdMotionInfo* vmdMotionInfo
	)
{
	// nullptr check in-param
	if (!DestSeq || !Skeleton || !vmdMotionInfo)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Error,
			TEXT("ImportVMDToAnimSequence : Ref InParam is Null. DestSeq[%x],Skelton[%x],vmdMotionInfo[%x]"),
			DestSeq, Skeleton, vmdMotionInfo );
		//TBD:: ERR in Param...
		return false;
	}
	if (!ReNameTable)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Warning,
			TEXT("ImportVMDToAnimSequence : Target ReNameTable is null."));
	}
	if (!mmdExtend)
	{
		UE_LOG(LogMMD4UE4_VMDFactory, Warning,
			TEXT("ImportVMDToAnimSequence : Target MMDExtendAsset is null."));
	}

	//UAnimDataController* uc = new UAnimDataController();
	/********************************/

	//double fps=DestSeq->GetFrameRate();
	auto &adc = DestSeq->GetController(); 

	const bool bShouldTransact = true;
	adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"), bShouldTransact);

	adc.InitializeModel();



	auto fr = FFrameRate(30, 1);
	adc.SetFrameRate(FFrameRate(30, 1));// DestSeq->SetRawNumberOfFrame(vmdMotionInfo->maxFrame);
	const FFrameNumber FrameNumber = fr.AsFrameNumber((vmdMotionInfo->maxFrame-1)/30.0);
	adc.SetNumberOfFrames(FrameNumber);
	
	//adc.SetPlayLength(FGenericPlatformMath::Max<float>(1.0f / 30.0f * (float)vmdMotionInfo->maxFrame, MINIMUM_ANIMATION_LENGTH));
	adc.NotifyPopulated();
	adc.CloseBracket();
	//DestSeq->SequenceLength = FGenericPlatformMath::Max<float>(1.0f / 30.0f * (float)vmdMotionInfo->maxFrame, MINIMUM_ANIMATION_LENGTH);
	/////////////////////////////////
	const int32 NumBones = Skeleton->GetReferenceSkeleton().GetNum();
#if 0 /* :UE414: 4.14からのエンジン仕様変更による対象 */
	DestSeq->RawAnimationData.AddZeroed(NumBones);
	DestSeq->AnimationTrackNames.AddUninitialized(NumBones);
#endif

	adc.OpenBracket(LOCTEXT("ImportAsSkeletalMesh", "Importing VMD Animation"), bShouldTransact);

	const TArray<FTransform>& RefBonePose = Skeleton->GetReferenceSkeleton().GetRefBonePose();

	TArray<FRawAnimSequenceTrack> TempRawTrackList;


	check(RefBonePose.Num() == NumBones);
	int mmdRoot = 0, mmdCenter=0;
	// SkeletonとのBone関係を登録する＠必須事項
	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
#if 0 /* :UE414: 4.14 エンジン仕様変更 */
		DestSeq->AnimationTrackNames[BoneIndex] = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);
		// add mapping to skeleton bone track
		DestSeq->TrackToSkeletonMapTable.Add(FTrackToSkeletonMap(BoneIndex));
#endif /* :UE414: */
		TempRawTrackList.Add(FRawAnimSequenceTrack());
		check(BoneIndex == TempRawTrackList.Num() - 1);
		FRawAnimSequenceTrack& RawTrack = TempRawTrackList[BoneIndex];

		auto refTranslation = RefBonePose[BoneIndex].GetTranslation();
		//★TBD:追加処理：以下検討中
		FName targetName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);;
		FName* pn = NameMap.Find(targetName);
		if (pn)
			targetName = *pn;
		if (!mmdRoot) {
			if (targetName == TEXT("全ての親"))
				mmdRoot = BoneIndex;
		} else if (!mmdCenter) {
			if (targetName == TEXT("センター"))
				mmdCenter = BoneIndex;
		}
		

		if (ReNameTable)
		{
			//もし変換テーブルのアセットを指定している場合はテーブルから変換名を取得する
			FMMD2UE4NameTableRow* dataRow;
			FString ContextString;
			dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(targetName, ContextString);
			if (dataRow)
			{
				targetName = FName(*dataRow->MmdOriginalName);
			}
		}

		int vmdKeyListIndex = vmdMotionInfo->FindKeyTrackName(targetName.ToString(), MMD4UE4::VmdMotionInfo::EVMD_KEYBONE);
		if (vmdKeyListIndex == -1)
		{
			{
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					TEXT("ImportVMDToAnimSequence Target Bone Not Found...[%s]"),
					*targetName.ToString());
			}
			//nop
			//フレーム分同じ値を設定する

			//for (int32 i = 0; i < DestSeq->GetNumberOfSampledKeys(); i++)
			//{
			//	FTransform nrmTrnc;
			//	nrmTrnc.SetIdentity();
			//	RawTrack.PosKeys.Add(FVector3f(nrmTrnc.GetTranslation() + refTranslation));
			//	RawTrack.RotKeys.Add(FQuat4f(nrmTrnc.GetRotation()));
			//	RawTrack.ScaleKeys.Add(FVector3f(nrmTrnc.GetScale3D()));
			//}
		}
		else
		{
			check(vmdKeyListIndex > -1);
			int sortIndex = 0;
			int preKeyIndex = -1;
			auto& kybone = vmdMotionInfo->keyBoneList[vmdKeyListIndex];
			//if (kybone.keyList.Num() < 2)					continue;
			int nextKeyIndex = kybone.sortIndexList[sortIndex];
			int nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;
			int baseKeyFrame = 0;

			{
				UE_LOG(LogMMD4UE4_VMDFactory, Log,
					TEXT("ImportVMDToAnimSequence Target Bone Found...Name[%s]-KeyNum[%d]"),
					*targetName.ToString(),
					kybone.sortIndexList.Num() );
			}
			//bool dbg = false;
			//if (targetName == L"右ひじ")
			//	dbg = true;
			//事前に各Trackに対し親Bone抜きにLocal座標で全登録予定のフレーム計算しておく（もっと良い処理があれば…検討）
			//90度以上の軸回転が入るとクォータニオンの為か処理に誤りがあるかで余計な回転が入ってしまう。
			//→上記により、単にZ回転（ターンモーション）で下半身と上半身の軸が物理的にありえない回転の組み合わせになる。バグ。
			
			auto nk = DestSeq->GetNumberOfSampledKeys();
			for (int32 i = 0; i < nk && nextKeyFrame>=i; i++)
			{
 
				if (i == 0)
				{
					if (i == nextKeyFrame)
					{
						FTransform tempTranceform(
							FQuat(
							kybone.keyList[nextKeyIndex].Quaternion[0],
							kybone.keyList[nextKeyIndex].Quaternion[2] * (-1),
							kybone.keyList[nextKeyIndex].Quaternion[1],
							kybone.keyList[nextKeyIndex].Quaternion[3]
							),
							FVector(
							kybone.keyList[nextKeyIndex].Position[0],
							kybone.keyList[nextKeyIndex].Position[2] * (-1),
							kybone.keyList[nextKeyIndex].Position[1]
							)*10.0f,
							FVector(1, 1, 1)
							);
						//リファレンスポーズからKeyのポーズ分移動させた値を初期値とする
						RawTrack.PosKeys.Add(FVector3f(tempTranceform.GetTranslation() + refTranslation));
						RawTrack.RotKeys.Add(FQuat4f(tempTranceform.GetRotation()));
						RawTrack.ScaleKeys.Add(FVector3f(tempTranceform.GetScale3D()));

						preKeyIndex = nextKeyIndex;
						uint32 lastKF = nextKeyFrame;
						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[nextKeyIndex].Frame <= lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];

						}
						lastKF = nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;

						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[kybone.sortIndexList[sortIndex + 1]].Frame == lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];

						}
						nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;

					}
					else
					{
						preKeyIndex = nextKeyIndex;
						//例外処理。初期フレーム(0)にKeyが設定されていない
						FTransform nrmTrnc;
						nrmTrnc.SetIdentity();
						RawTrack.PosKeys.Add(FVector3f(nrmTrnc.GetTranslation()+ refTranslation));
						RawTrack.RotKeys.Add(FQuat4f(nrmTrnc.GetRotation()));
						RawTrack.ScaleKeys.Add(FVector3f(nrmTrnc.GetScale3D()));
					}
				}
				else // if (nextKeyFrame == i)
				{
					float blendRate = 1;
					FTransform NextTranc;
					FTransform PreTranc;
					FTransform NowTranc;

					NextTranc.SetIdentity();
					PreTranc.SetIdentity();
					NowTranc.SetIdentity();

					if (nextKeyIndex > 0)
					{
						MMD4UE4::VMD_KEY & PreKey = kybone.keyList[preKeyIndex];
						MMD4UE4::VMD_KEY & NextKey = kybone.keyList[nextKeyIndex];
						if (NextKey.Frame <= (uint32)i)
						{
							blendRate = 1.0f;
						}
						else
						{ 
							//TBD::フレーム間が1だと0.5で計算されない?
							blendRate = 1.0f - (float)(NextKey.Frame - (uint32)i) / (float)(NextKey.Frame - PreKey.Frame);
						}
						//pose
						NextTranc.SetLocation(
							FVector(
							NextKey.Position[0],
							NextKey.Position[2] * (-1),
							NextKey.Position[1]
							));
						PreTranc.SetLocation(
							FVector(
							PreKey.Position[0],
							PreKey.Position[2] * (-1),
							PreKey.Position[1]
							));

						NowTranc.SetLocation(
							FVector(
							interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_X] / 127.0f,
							blendRate
							)*(NextTranc.GetTranslation().X - PreTranc.GetTranslation().X) + PreTranc.GetTranslation().X
							,
							interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_Z] / 127.0f,
							blendRate
							)*(NextTranc.GetTranslation().Y - PreTranc.GetTranslation().Y) + PreTranc.GetTranslation().Y
							,
							interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_Y] / 127.0f,
							blendRate
							)*(NextTranc.GetTranslation().Z - PreTranc.GetTranslation().Z) + PreTranc.GetTranslation().Z
							)
							);
						//rot
						NextTranc.SetRotation(
							FQuat(
							NextKey.Quaternion[0],
							NextKey.Quaternion[2] * (-1),
							NextKey.Quaternion[1],
							NextKey.Quaternion[3]
							));
						PreTranc.SetRotation(
							FQuat(
							PreKey.Quaternion[0],
							PreKey.Quaternion[2] * (-1),
							PreKey.Quaternion[1],
							PreKey.Quaternion[3]
							));
#if 0
						float tempBezR[4];
						NowTranc.SetRotation(
							FQuat(
							tempBezR[0] = interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							blendRate
							)*(NextTranc.GetRotation().X - PreTranc.GetRotation().X) + PreTranc.GetRotation().X
							,
							tempBezR[1] = interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							blendRate
							)*(NextTranc.GetRotation().Y - PreTranc.GetRotation().Y) + PreTranc.GetRotation().Y
							,
							tempBezR[2] = interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							blendRate
							)*(NextTranc.GetRotation().Z - PreTranc.GetRotation().Z) + PreTranc.GetRotation().Z
							,
							tempBezR[3] = interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							blendRate
							)*(NextTranc.GetRotation().W - PreTranc.GetRotation().W) + PreTranc.GetRotation().W
							)
							);
						UE_LOG(LogMMD4UE4_VMDFactory, Warning,
							TEXT("interpolateBezier Rot:[%s],F[%d/%d],BLD[%.2f],BEZ[%.3f][%.3f][%.3f][%.3f]"),
							*targetName.ToString(), i, NextKey.Frame, blendRate, tempBezR[0], tempBezR[1], tempBezR[2], tempBezR[3]
							);
#else
						float bezirT = interpolateBezier(
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_0][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_X][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							NextKey.Bezier[D_VMD_KEY_BEZIER_AR_0_BEZ_1][D_VMD_KEY_BEZIER_AR_1_BEZ_Y][D_VMD_KEY_BEZIER_AR_2_KND_R] / 127.0f,
							blendRate
							);
						NowTranc.SetRotation(
							FQuat::Slerp(PreTranc.GetRotation(), NextTranc.GetRotation(), bezirT)
							);
						/*UE_LOG(LogMMD4UE4_VMDFactory, Warning,
							TEXT("interpolateBezier Rot:[%s],F[%d/%d],BLD[%.2f],biz[%.2f]BEZ[%s]"),
							*targetName.ToString(), i, NextKey.Frame, blendRate, bezirT,*NowTranc.GetRotation().ToString()
							);*/
#endif
					}
					else
					{
						NowTranc.SetLocation(
							FVector(
							kybone.keyList[nextKeyIndex].Position[0],
							kybone.keyList[nextKeyIndex].Position[2] * (-1),
							kybone.keyList[nextKeyIndex].Position[1]
							));
						NowTranc.SetRotation(
							FQuat(
							kybone.keyList[nextKeyIndex].Quaternion[0],
							kybone.keyList[nextKeyIndex].Quaternion[2] * (-1),
							kybone.keyList[nextKeyIndex].Quaternion[1],
							kybone.keyList[nextKeyIndex].Quaternion[3]
							));
						//TBD:このルートが存在する模様、処理の再検討が必要
						//check(false);
					}

					FTransform tempTranceform(
						NowTranc.GetRotation(),
						NowTranc.GetTranslation()*10.0f,
						FVector(1, 1, 1)
						);
					//リファレンスポーズからKeyのポーズ分移動させた値を初期値とする
					auto poskey = FVector3f(tempTranceform.GetTranslation() + refTranslation);
					RawTrack.PosKeys.Add(poskey);
					RawTrack.RotKeys.Add(FQuat4f(tempTranceform.GetRotation()));
					RawTrack.ScaleKeys.Add(FVector3f(tempTranceform.GetScale3D()));
					//if (BoneIndex==3)
					//UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					//	TEXT("P %8.2f,  %8.2f,  %8.2f"),
					//	poskey.X, poskey.Y, poskey.Z
					//);

					if (nextKeyFrame == i)
					{
#if 0
						//該当キーと一致。直値代入
						//RawTrack.PosKeys.Add()
						if (sortIndex + 1 < kybone.sortIndexList.Num())
						{
							sortIndex++;
							preKeyIndex = nextKeyIndex;
							nextKeyIndex = kybone.sortIndexList[sortIndex];
							nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;
						}
						else
						{
							//TDB::Last Frame, not ++
						}
#else
						preKeyIndex = nextKeyIndex;
						uint32 lastKF = nextKeyFrame;
						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[nextKeyIndex].Frame <= lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];

						}
						lastKF = nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;

						while (sortIndex + 1 < kybone.sortIndexList.Num() && kybone.keyList[kybone.sortIndexList[sortIndex+1]].Frame == lastKF)
						{
							sortIndex++;
							nextKeyIndex = kybone.sortIndexList[sortIndex];

						}
						nextKeyFrame = kybone.keyList[nextKeyIndex].Frame;
#endif
					}
				}
			}
		}

		
	}

#if 0
	////////////////////////////////////
	//親ボーンの順番::TBD(IK用)
	TArray<int> TempBoneTreeRefSkel;
	int tempParentID = -1;
	while (TempBoneTreeRefSkel.Num() == NumBones)
	{
		for (int i = 0; i < NumBones; ++i)
		{
			//if (tempParentID == )
			{
				TempBoneTreeRefSkel.Add(i);
			}
		}
	}

	//全フレームに対し事前に各ボーンの座標を計算しRawTrackに追加する＠必須
	//TBD::多分バグがあるはず…
	FTransform subLocRef;
	FTransform tempLoc;
	TArray<FRawAnimSequenceTrack> ImportRawTrackList;
	ImportRawTrackList.AddZeroed(NumBones);
	GWarn->BeginSlowTask(LOCTEXT("BeginImportAnimation", "Importing Animation"), true);

	for (int32 k = 0; k < DestSeq->GetNumberOfSampledKeys(); k++)
	{
		// update status
		FFormatNamedArguments Args;
		//Args.Add(TEXT("TrackName"), FText::FromName(BoneName));
		Args.Add(TEXT("NowKey"), FText::AsNumber(k));
		Args.Add(TEXT("TotalKey"), FText::AsNumber(DestSeq->GetNumberOfSampledKeys()));
		//Args.Add(TEXT("TrackIndex"), FText::AsNumber(SourceTrackIdx + 1));
		Args.Add(TEXT("TotalTracks"), FText::AsNumber(NumBones));
		//const FText StatusUpate = FText::Format(LOCTEXT("ImportingAnimTrackDetail", "Importing Animation Track [{TrackName}] ({TrackIndex}/{TotalTracks}) - TotalKey {TotalKey}"), Args);
		const FText StatusUpate
			= FText::Format(LOCTEXT("ImportingAnimTrackDetail",
				"Importing Animation Track Key({NowKey}/{TotalKey}) - TotalTracks {TotalTracks}"),
				Args);
		GWarn->StatusForceUpdate(k, DestSeq->GetNumberOfSampledKeys(), StatusUpate);

		for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
		{
#if 1 /* :UE414: 4.14からのエンジン仕様変更 */
			FRawAnimSequenceTrack &RawTrack = ImportRawTrackList[BoneIndex];
						
#else	/* ~UE4.13 */
			DestSeq->AnimationTrackNames[BoneIndex] = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);

			FRawAnimSequenceTrack& RawTrack = DestSeq->RawAnimationData[BoneIndex];
#endif
			FRawAnimSequenceTrack& LocalRawTrack = TempRawTrackList[BoneIndex];

			/////////////////////////////////////
			///お試し、親ボーンの順番を考えない
			{
				//移動後の位置 = 元の位置 + 平行移動
				//移動後の回転 = 回転
				RawTrack.PosKeys.Add(LocalRawTrack.PosKeys[k] + RefBonePose[BoneIndex].GetTranslation());
				RawTrack.RotKeys.Add(LocalRawTrack.RotKeys[k]);
				RawTrack.ScaleKeys.Add(LocalRawTrack.ScaleKeys[k]);
			}
		}
	}
#endif
#if 0 /* :UE414: 4.14でAnimationの登録仕様が変わったため、いったん機能制限とする */
	if(mmdExtend)
	{
		FName targetName;
		// ik Target loop int setup ...
		for (int32 ikTargetIndex = 0; ikTargetIndex < mmdExtend->IkInfoList.Num(); ++ikTargetIndex)
		{
			//check bone has skeleton .
			mmdExtend->IkInfoList[ikTargetIndex].checkIKIndex = true;
			//vmd
			//★TBD:追加処理：以下検討中
			targetName = mmdExtend->IkInfoList[ikTargetIndex].IKBoneName;
			if (ReNameTable)
			{
				//もし変換テーブルのアセットを指定している場合はテーブルから変換名を取得する
				FMMD2UE4NameTableRow* dataRow;
				FString ContextString;
				dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(targetName, ContextString);
				if (dataRow)
				{
					targetName = FName(*dataRow->MmdOriginalName);
				}
			}
			if ((mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndexVMDKey
				= vmdMotionInfo->FindKeyTrackName(targetName.ToString(), MMD4UE4::VmdMotionInfo::EVMD_KEYBONE)) == -1)
			{
				mmdExtend->IkInfoList[ikTargetIndex].checkIKIndex = false;
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					TEXT("IKBoneIndexVMDKey index (skelton) not found...name[%s]"),
					*targetName.ToString()
					);
				//not found ik key has this vmd data.
				continue; 
			}

			//search skeleton bone index
			//ik bone
			if ((mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex
				= FindRefBoneInfoIndexFromBoneName(Skeleton->GetReferenceSkeleton(), targetName)) == -1)
			{
				mmdExtend->IkInfoList[ikTargetIndex].checkIKIndex = false;
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					TEXT("IKBoneIndex index (skelton) not found...name[%s]"),
					*targetName.ToString()
					);
				continue;
			}
			//ik target bone
			targetName = mmdExtend->IkInfoList[ikTargetIndex].TargetBoneName;
			if (ReNameTable)
			{
				//もし変換テーブルのアセットを指定している場合はテーブルから変換名を取得する
				FMMD2UE4NameTableRow* dataRow;
				FString ContextString;
				dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(targetName, ContextString);
				if (dataRow)
				{
					targetName = FName(*dataRow->MmdOriginalName);
				}
			}
			if ((mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex
				= FindRefBoneInfoIndexFromBoneName(Skeleton->GetReferenceSkeleton(), targetName)) == -1)
			{
				mmdExtend->IkInfoList[ikTargetIndex].checkIKIndex = false;
				UE_LOG(LogMMD4UE4_VMDFactory, Warning,
					TEXT("TargetBoneIndex index (skelton) not found...name[%s]"),
					*targetName.ToString()
					);
				continue;
			}
			//loop sub bone
			for (int32 subBone = 0; subBone < mmdExtend->IkInfoList[ikTargetIndex].ikLinkList.Num(); ++subBone)
			{

				//ik target bone
				targetName = mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[subBone].BoneName;
				if (ReNameTable)
				{
					//もし変換テーブルのアセットを指定している場合はテーブルから変換名を取得する
					FMMD2UE4NameTableRow* dataRow;
					FString ContextString;
					dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(targetName, ContextString);
					if (dataRow)
					{
						targetName = FName(*dataRow->MmdOriginalName);
					}
				}
				if((mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[subBone].BoneIndex
					= FindRefBoneInfoIndexFromBoneName(Skeleton->GetReferenceSkeleton(), targetName)) == -1)
				{
					mmdExtend->IkInfoList[ikTargetIndex].checkIKIndex = false;
					UE_LOG(LogMMD4UE4_VMDFactory, Warning,
						TEXT("sub BoneIndex index (skelton) not found...name[%s],sub[%d]"),
						*targetName.ToString(),
						subBone
						);
					continue;
				}
			}
		}
		//TBD::ここにCCD-IK再計算処理
		//案1：各フレーム計算中に逐次処理
		//案2：FKを全フレーム計算完了後にIKだけまとめてフレーム単位で再計算

		//
		for (int32 k = 0; k < DestSeq->NumFrames; k++)
		{
			// ik Target loop func...
			for (int32 ikTargetIndex = 0; ikTargetIndex < mmdExtend->IkInfoList.Num(); ++ikTargetIndex)
			{
				//check 
				if (!mmdExtend->IkInfoList[ikTargetIndex].checkIKIndex)
				{
					/*UE_LOG(LogMMD4UE4_VMDFactory, Warning,
						TEXT("IK func skip: Parameter is insufficient.[%s]..."),
						*mmdExtend->IkInfoList[ikTargetIndex].IKBoneName.ToString()
						);*/
					continue;
				}
				int32 loopMax = mmdExtend->IkInfoList[ikTargetIndex].LoopNum;
				int32 ilLinklistNum = mmdExtend->IkInfoList[ikTargetIndex].ikLinkList.Num();
				//TBD::もっと良い方があれば検討(Loc→Glbに再計算している為、計算コストが高すぎると推測)
				//ここで事前にIK対象のGlb座標等を計算しておく
				FTransform tempGlbIkBoneTrsf;
				FTransform tempGlbTargetBoneTrsf;
				TArray<FTransform> tempGlbIkLinkTrsfList;
				//get glb trsf
				/*DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex].PosKeys[k]
					= RefBonePose[mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex].GetTranslation();
				DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex].RotKeys[k]
					= RefBonePose[mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex].GetRotation();
				DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex].ScaleKeys[k]
					= FVector(1);*/
				tempGlbIkBoneTrsf = CalcGlbTransformFromBoneIndex(
					DestSeq,
					Skeleton,
					mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex,
					k
					);
				/*DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex].PosKeys[k]
					= RefBonePose[mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex].GetTranslation();
				DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex].RotKeys[k]
					= RefBonePose[mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex].GetRotation();
				DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex].ScaleKeys[k]
					= FVector(1);*/
				tempGlbTargetBoneTrsf = CalcGlbTransformFromBoneIndex(
					DestSeq,
					Skeleton,
					mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex,
					k
					);
				tempGlbIkLinkTrsfList.AddZeroed(ilLinklistNum);
				for (int32 glbIndx = 0; glbIndx < ilLinklistNum; ++glbIndx)
				{
					/*DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex].PosKeys[k]
						= RefBonePose[mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex].GetTranslation();
					DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex].RotKeys[k]
						= RefBonePose[mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex].GetRotation();
					DestSeq->RawAnimationData[mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex].ScaleKeys[k]
						= FVector(1);*/
					tempGlbIkLinkTrsfList[glbIndx] = CalcGlbTransformFromBoneIndex(
						DestSeq,
						Skeleton,
						mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex,
						k
						);
				}
				//execute 
				//
				FTransform tempCalcIKTrns;
				tempCalcIKTrns.SetIdentity();
				//
				FVector vecToIKTargetPose;
				FVector vecToIKLinkPose;
				FVector asix;
				FQuat	qt;
				float	angle = 0;
				int32	rawIndex = 0;
				int32	ikt = loopMax / 2;
				for (int32 loopCount = 0; loopCount < loopMax; ++loopCount)
				{
					for (int32 linkBoneIndex = 0; linkBoneIndex < ilLinklistNum; ++linkBoneIndex)
					{
						rawIndex = mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex].BoneIndex;
#if 0 //test ik bug
						////////////////////
						// Ref: http://pr0jectze10.blogspot.jp/2011/06/ik-part2.html
						////////////////////
						// IKターゲットボーンまでのベクトル
						vecToIKTargetPose
							= tempGlbTargetBoneTrsf.GetTranslation() - tempGlbIkLinkTrsfList[linkBoneIndex].GetTranslation();
						// IKボーンまでのベクトル 
						vecToIKLinkPose
							= tempGlbIkBoneTrsf.GetTranslation() - tempGlbIkLinkTrsfList[linkBoneIndex].GetTranslation();

						// 2つのベクトルの外積（上軸）を算出 
						asix = FVector::CrossProduct(vecToIKTargetPose, vecToIKLinkPose);
						// 軸の数値が正常で、2つのベクトルの向きが不一致の場合（ベクトルの向きが同じ場合、外積は0になる）
						if (asix.SizeSquared()>0)
						{
							// 法線化
							asix.Normalize();
							vecToIKTargetPose.Normalize();
							vecToIKLinkPose.Normalize();
							// 2つのベクトルの内積を計算して、ベクトル間のラジアン角度を算出 
							angle = FMath::Acos(FMath::Clamp<float>( FVector::DotProduct(vecToIKTargetPose, vecToIKLinkPose),-1,1));
							//angle = FVector::DotProduct(vecToIKTargetPose, vecToIKLinkPose);
							float RotLimitRad = FMath::DegreesToRadians(mmdExtend->IkInfoList[ikTargetIndex].RotLimit);
							if (angle > RotLimitRad)
							{
								angle = RotLimitRad;
							}
							// 任意軸回転クォータニオンを作成  
							qt = FQuat(asix, angle);
							// 変換行列に合成 
							tempCalcIKTrns.SetIdentity();
							//tempCalcIKTrns.SetTranslation(DestSeq->RawAnimationData[rawIndex].PosKeys[k]);
							tempCalcIKTrns.SetRotation(DestSeq->RawAnimationData[rawIndex].RotKeys[k]);
							tempCalcIKTrns *= FTransform(qt);

							//軸制限計算
							if (mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex].RotLockFlag == 1)
							{
								FVector eulerVec = tempCalcIKTrns.GetRotation().Euler();
								FVector subEulerAngleVec = eulerVec;

								subEulerAngleVec = ClampVector(
									subEulerAngleVec,
									mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex].RotLockMin,
									mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex].RotLockMax
									);
								subEulerAngleVec -= eulerVec;
								//回転軸制限補正
								tempCalcIKTrns *= FTransform(FQuat::MakeFromEuler(subEulerAngleVec));
							}
							//CCD-IK後の回転軸更新
							DestSeq->RawAnimationData[rawIndex].RotKeys[k]
								= tempCalcIKTrns.GetRotation();
#if 0
							UE_LOG(LogMMD4UE4_VMDFactory, Warning,
								TEXT("test.[%d][%s][%lf/%lf]..."),
								k,
								*mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex].BoneName.ToString(),
								angle,
								angle*180/3.14
								);
#endif
							// recalc glb trans
							tempGlbIkBoneTrsf = CalcGlbTransformFromBoneIndex(
								DestSeq,
								Skeleton,
								mmdExtend->IkInfoList[ikTargetIndex].IKBoneIndex,
								k
								);
							tempGlbTargetBoneTrsf = CalcGlbTransformFromBoneIndex(
								DestSeq,
								Skeleton,
								mmdExtend->IkInfoList[ikTargetIndex].TargetBoneIndex,
								k
								);
							tempGlbIkLinkTrsfList.AddZeroed(ilLinklistNum);
							for (int32 glbIndx = 0; glbIndx < ilLinklistNum; ++glbIndx)
							{
								tempGlbIkLinkTrsfList[glbIndx] = CalcGlbTransformFromBoneIndex(
									DestSeq,
									Skeleton,
									mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[glbIndx].BoneIndex,
									k
									);
							}
							/////////////
						}
#else
						//////////////////////////////////////////////////////////
						// test
						//from dxlib pmx ik, kai
						//
						//and
						//ref: http://marupeke296.com/DXG_No20_TurnUsingQuaternion.html
						// j = linkBoneIndex
						//////////////////////////////
						FMMD_IKLINK * IKBaseLinkPtr = &mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex];
						// IKターゲットボーンまでのベクトル
						vecToIKTargetPose = tempGlbIkLinkTrsfList[linkBoneIndex].GetTranslation();
						vecToIKTargetPose -= tempGlbTargetBoneTrsf.GetTranslation();
						// IKボーンまでのベクトル 
						vecToIKLinkPose = tempGlbIkLinkTrsfList[linkBoneIndex].GetTranslation();
						vecToIKLinkPose -= tempGlbIkBoneTrsf.GetTranslation();
						// 法線化
						vecToIKTargetPose.Normalize();
						vecToIKLinkPose.Normalize();

						FVector v1, v2;
						v1 = vecToIKTargetPose;
						v2 = vecToIKLinkPose;
						if ((v1.X - v2.X) * (v1.X - v2.X)
							+ (v1.Y - v2.Y) * (v1.Y - v2.Y)
							+ (v1.Z - v2.Z) * (v1.Z - v2.Z) 
							< 0.0000001f) break;

						FVector v;
						v = FVector::CrossProduct(v1, v2);
						// calculate roll axis
						//親計算：無駄計算を省きたいがうまい方法が見つからないので保留
						FVector ChainParentBone_Asix;
						FTransform tempGlbChainParentBoneTrsf = CalcGlbTransformFromBoneIndex(
							DestSeq,
							Skeleton,
							Skeleton->GetReferenceSkeleton().GetParentIndex(IKBaseLinkPtr->BoneIndex),//parent
							k
							);
						if (IKBaseLinkPtr->RotLockFlag == 1 && loopCount < ikt)
						{
							if (IKBaseLinkPtr->RotLockMin.Y == 0 && IKBaseLinkPtr->RotLockMax.Y == 0
								&& IKBaseLinkPtr->RotLockMin.Z == 0 && IKBaseLinkPtr->RotLockMax.Z == 0)
							{
								ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::X);
								float vvx = FVector::DotProduct(v, ChainParentBone_Asix);
								v.X = vvx >= 0.0f ? 1.0f : -1.0f;
								v.Y = 0.0f;
								v.Z = 0.0f;
							}
							else if (IKBaseLinkPtr->RotLockMin.X == 0 && IKBaseLinkPtr->RotLockMax.X == 0
								&& IKBaseLinkPtr->RotLockMin.Z == 0 && IKBaseLinkPtr->RotLockMax.Z == 0)
							{
								ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::Y);
								float vvy = FVector::DotProduct(v, ChainParentBone_Asix);
								v.Y = vvy >= 0.0f ? 1.0f : -1.0f;
								v.X = 0.0f;
								v.Z = 0.0f;
							}
							else if (IKBaseLinkPtr->RotLockMin.X == 0 && IKBaseLinkPtr->RotLockMax.X == 0
								&& IKBaseLinkPtr->RotLockMin.Y == 0 && IKBaseLinkPtr->RotLockMax.Y == 0)
							{
								ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::Z);
								float vvz = FVector::DotProduct(v, ChainParentBone_Asix);
								v.Z = vvz >= 0.0f ? 1.0f : -1.0f;
								v.X = 0.0f;
								v.Y = 0.0f;
							}
							else
							{
								// calculate roll axis
								FVector vv;

								ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::X);
								vv.X = FVector::DotProduct(v, ChainParentBone_Asix);
								ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::Y);
								vv.Y = FVector::DotProduct(v, ChainParentBone_Asix);
								ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::Z);
								vv.Z = FVector::DotProduct(v, ChainParentBone_Asix);

								v = vv;
								v.Normalize();
							}
						}
						else
						{
							// calculate roll axis
							FVector vv;

							ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::X);
							vv.X = FVector::DotProduct(v, ChainParentBone_Asix);
							ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::Y);
							vv.Y = FVector::DotProduct(v, ChainParentBone_Asix);
							ChainParentBone_Asix = tempGlbChainParentBoneTrsf.GetUnitAxis(EAxis::Z);
							vv.Z = FVector::DotProduct(v, ChainParentBone_Asix);

							v = vv;
							v.Normalize();
						}


						// calculate roll angle of [k]th bone(limited by p_IK[i].dlimit*(k+1)*2)
						float Cos = FVector::DotProduct(v1,v2);
						if (Cos >  1.0f) Cos = 1.0f;
						if (Cos < -1.0f) Cos = -1.0f;

						float Rot = 0.5f * FMath::Acos(Cos);
						float RotLimitRad = FMath::DegreesToRadians(mmdExtend->IkInfoList[ikTargetIndex].RotLimit);
						FVector RotLockMinRad = IKBaseLinkPtr->RotLockMin * FMath::DegreesToRadians(1);
						FVector RotLockMaxRad = IKBaseLinkPtr->RotLockMax * FMath::DegreesToRadians(1);
						//TBD::単位角度の制限確認(ただし、処理があっているか不明…）
						if (Rot > RotLimitRad * (linkBoneIndex + 1) * 2)
						{
							Rot = RotLimitRad * (linkBoneIndex + 1) * 2;
						}
						/* 暫定：Dxlib版
						float IKsin, IKcos;
						IKsin = FMath::Sin(Rot);
						IKcos = FMath::Cos(Rot);
						FQuat qIK(
							v.X * IKsin,
							v.Y * IKsin,
							v.Z * IKsin,
							IKcos
							);	*/
						//UE4版：軸と角度からQuar作成
						FQuat qIK(v, Rot);

						//chainBone ik quatがGlbかLocかIKのみか不明。・・暫定
						FQuat ChainBone_IKQuat = tempGlbIkBoneTrsf.GetRotation();
						ChainBone_IKQuat = qIK * ChainBone_IKQuat ;
						tempGlbIkBoneTrsf.SetRotation(ChainBone_IKQuat);

						FMatrix ChainBone_IKmat;
						ChainBone_IKmat = tempGlbIkBoneTrsf.ToMatrixNoScale(); //時たまここでクラッシュする・・・解決策を探す。

						//軸制限計算
						if (IKBaseLinkPtr->RotLockFlag == 1)
						{
							// 軸回転角度を算出
							if ((RotLockMinRad.X > -1.570796f) & (RotLockMaxRad.X < 1.570796f))
							{
								// Z*X*Y順
								// X軸回り
								float fLimit = 1.535889f;			// 88.0f/180.0f*3.14159265f;
								float fSX = -ChainBone_IKmat.M[2][1];				// sin(θx)
								float fX = FMath::Asin(fSX);			// X軸回り決定
								float fCX = FMath::Cos(fX);

								// ジンバルロック回避
								if (FMath::Abs<float>(fX) > fLimit)
								{
									fX = (fX < 0) ? -fLimit : fLimit; 
									fCX = FMath::Cos(fX);
								}

								// Y軸回り
								float fSY = ChainBone_IKmat.M[2][0] / fCX;
								float fCY = ChainBone_IKmat.M[2][2] / fCX;
								float fY = FMath::Atan2(fSY, fSX);	// Y軸回り決定

								// Z軸回り
								float fSZ = ChainBone_IKmat.M[0][1] / fCX;
								float fCZ = ChainBone_IKmat.M[1][1] / fCX;
								float fZ = FMath::Atan2(fSZ, fCZ);

								// 角度の制限
								FVector fixRotAngleVec(fX,fY,fZ);
								CheckLimitAngle(
									RotLockMinRad,
									RotLockMaxRad,
									&fixRotAngleVec, 
									(linkBoneIndex < ikt)
									);
								// 決定した角度でベクトルを回転
								FMatrix mX, mY, mZ, mT;

								CreateRotationXMatrix(&mX, fixRotAngleVec.X);
								CreateRotationXMatrix(&mY, fixRotAngleVec.Y);
								CreateRotationXMatrix(&mZ, fixRotAngleVec.Z);

								MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(&mT, &mZ, &mX);
								MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(&ChainBone_IKmat, &mT, &mY);
							}
							else if ((RotLockMinRad.X > -1.570796f) & (RotLockMaxRad.X < 1.570796f))
							{
								// X*Y*Z順
								// Y軸回り
								float fLimit = 1.535889f;		// 88.0f/180.0f*3.14159265f;
								float fSY = -ChainBone_IKmat.M[0][2];			// sin(θy)
								float fY = FMath::Asin(fSY);	// Y軸回り決定
								float fCY = FMath::Cos(fY);

								// ジンバルロック回避
								if (FMath::Abs<float>(fY) > fLimit)
								{
									fY = (fY < 0) ? -fLimit : fLimit;
									fCY = FMath::Cos(fY);
								}

								// X軸回り
								float fSX = ChainBone_IKmat.M[1][2] / fCY;
								float fCX = ChainBone_IKmat.M[2][2] / fCY;
								float fX = FMath::Atan2(fSX, fCX);	// X軸回り決定

								// Z軸回り
								float fSZ = ChainBone_IKmat.M[0][1] / fCY;
								float fCZ = ChainBone_IKmat.M[0][0] / fCY;
								float fZ = FMath::Atan2(fSZ, fCZ);	// Z軸回り決定

								// 角度の制限
								FVector fixRotAngleVec(fX, fY, fZ);
								CheckLimitAngle(
									RotLockMinRad,
									RotLockMaxRad,
									&fixRotAngleVec,
									(linkBoneIndex < ikt)
									);

								// 決定した角度でベクトルを回転
								FMatrix mX, mY, mZ, mT;

								CreateRotationXMatrix(&mX, fixRotAngleVec.X);
								CreateRotationXMatrix(&mY, fixRotAngleVec.Y);
								CreateRotationXMatrix(&mZ, fixRotAngleVec.Z);

								MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(&mT, &mX, &mY);
								MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(&ChainBone_IKmat, &mT, &mZ);
							}
							else
							{
								// Y*Z*X順
								// Z軸回り
								float fLimit = 1.535889f;		// 88.0f/180.0f*3.14159265f;
								float fSZ = -ChainBone_IKmat.M[1][0];			// sin(θy)
								float fZ = FMath::Asin(fSZ);	// Y軸回り決定
								float fCZ = FMath::Cos(fZ);

								// ジンバルロック回避
								if (FMath::Abs(fZ) > fLimit)
								{
									fZ = (fZ < 0) ? -fLimit : fLimit;
									fCZ = FMath::Cos(fZ);
								}

								// X軸回り
								float fSX = ChainBone_IKmat.M[1][2] / fCZ;
								float fCX = ChainBone_IKmat.M[1][1] / fCZ;
								float fX = FMath::Atan2(fSX, fCX);	// X軸回り決定

								// Y軸回り
								float fSY = ChainBone_IKmat.M[2][0] / fCZ;
								float fCY = ChainBone_IKmat.M[0][0] / fCZ;
								float fY = FMath::Atan2(fSY, fCY);	// Z軸回り決定
								
								// 角度の制限
								FVector fixRotAngleVec(fX, fY, fZ);
								CheckLimitAngle(
									RotLockMinRad,
									RotLockMaxRad,
									&fixRotAngleVec,
									(linkBoneIndex < ikt)
									);

								// 決定した角度でベクトルを回転
								FMatrix mX, mY, mZ, mT;

								CreateRotationXMatrix(&mX, fixRotAngleVec.X);
								CreateRotationXMatrix(&mY, fixRotAngleVec.Y);
								CreateRotationXMatrix(&mZ, fixRotAngleVec.Z);

								MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(&mT, &mY, &mZ);
								MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(&ChainBone_IKmat, &mT, &mX);
							}
							//QuatConvertFromMatrix(ChainBone->IKQuat, ChainBone->IKmat);
							tempGlbIkBoneTrsf.SetFromMatrix(ChainBone_IKmat);
							tempGlbIkBoneTrsf.SetScale3D(FVector(1));//reset scale
							DestSeq->RawAnimationData[rawIndex].RotKeys[k]
								= tempGlbIkBoneTrsf.GetRotation();
#if 0
							UE_LOG(LogMMD4UE4_VMDFactory, Warning,
								TEXT("test.Loop[%d]LinkIndx[%d]Ke[%d]Nam[%s]Trf[%s]..."),
								loopCount,
								linkBoneIndex,
								k,
								*mmdExtend->IkInfoList[ikTargetIndex].ikLinkList[linkBoneIndex].BoneName.ToString(),
								*tempGlbIkBoneTrsf.ToString()
								);
							UE_LOG(LogMMD4UE4_VMDFactory, Error,
								TEXT("ikmat[%s]..."),
								*ChainBone_IKmat.ToString()
								);
#endif
						}
						else
						{
							//non limit
							DestSeq->RawAnimationData[rawIndex].RotKeys[k]
								= tempGlbIkBoneTrsf.GetRotation();
						}
						//////////////////////////////////////////////////
#endif
					}
				}
			}
		}
	}
#endif /* :UE414: 4.14のAnimationクラス仕様変更による機能制限 */

	/* AddTrack */
	if (!USE_RM || mmdCenter == 0) {
		for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
		{
			FName BoneName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);

			FRawAnimSequenceTrack& RawTrack = TempRawTrackList[BoneIndex];

			//DestSeq->AddNewRawTrack(BoneName, &RawTrack);

			{
				if (RawTrack.PosKeys.Num() > 1)
				{

					if (adc.AddBoneCurve(BoneName))
					{
						adc.SetBoneTrackKeys(BoneName, RawTrack.PosKeys, RawTrack.RotKeys, RawTrack.ScaleKeys);
					}
				}
			}
		}
	}
	else {

		auto& rootTrack = TempRawTrackList[mmdRoot];
		auto& cetrTrack = TempRawTrackList[mmdCenter];

		if (rootTrack.PosKeys.Num() < cetrTrack.PosKeys.Num()) {
			auto pos = rootTrack.PosKeys.Last();
			auto rot = rootTrack.RotKeys.Last();
			auto scl = rootTrack.ScaleKeys.Last();
			int n = cetrTrack.PosKeys.Num() - rootTrack.PosKeys.Num();
			for (int i = 0; i < n; i++) {
				rootTrack.PosKeys.Add(pos);
				rootTrack.RotKeys.Add(rot);
				rootTrack.ScaleKeys.Add(scl);
			}
		}
		
		TempRawTrackList[0] = TempRawTrackList[mmdRoot];
		for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
		{
			FName BoneName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneIndex);

			FRawAnimSequenceTrack& RawTrack = TempRawTrackList[BoneIndex];

			{
				if (BoneIndex == 0) {
					for (int i = 0; i < RawTrack.PosKeys.Num(); i++) {
						RawTrack.PosKeys[i].X += cetrTrack.PosKeys[i].X; cetrTrack.PosKeys[i].X = 0;
						RawTrack.PosKeys[i].Y += cetrTrack.PosKeys[i].Y; cetrTrack.PosKeys[i].Y = 0;
					}
				}
				
				if (RawTrack.PosKeys.Num() > 1 && BoneIndex!=mmdRoot)
				{

					if (adc.AddBoneCurve(BoneName))
					{
						adc.SetBoneTrackKeys(BoneName, RawTrack.PosKeys, RawTrack.RotKeys, RawTrack.ScaleKeys);
					}
				}
			}
		}
	}
	adc.NotifyPopulated();
	adc.CloseBracket();
	
	//GWarn->EndSlowTask();
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////
// Ｘ軸を中心とした回転行列を作成する
void CreateRotationXMatrix(FMatrix *Out, float Angle)
{
	float Sin, Cos;

	//_SINCOS(Angle, &Sin, &Cos);
	//	Sin = sinf( Angle ) ;
	//	Cos = cosf( Angle ) ;
	Sin = FMath::Sin(Angle);
	Cos = FMath::Cos(Angle);

	//_MEMSET(Out, 0, sizeof(MATRIX));
	FMemory::Memzero(Out,sizeof(FMatrix));
	Out->M[0][0] = 1.0f;
	Out->M[1][1] = Cos;
	Out->M[1][2] = Sin;
	Out->M[2][1] = -Sin;
	Out->M[2][2] = Cos;
	Out->M[3][3] = 1.0f;

	//return 0;
}
// 回転成分だけの行列の積を求める( ３×３以外の部分には値も代入しない )
void MV1LoadModelToVMD_CreateMultiplyMatrixRotOnly(FMatrix *Out, FMatrix *In1, FMatrix *In2)
{
	Out->M[0][0] = In1->M[0][0] * In2->M[0][0] + In1->M[0][1] * In2->M[1][0] + In1->M[0][2] * In2->M[2][0];
	Out->M[0][1] = In1->M[0][0] * In2->M[0][1] + In1->M[0][1] * In2->M[1][1] + In1->M[0][2] * In2->M[2][1];
	Out->M[0][2] = In1->M[0][0] * In2->M[0][2] + In1->M[0][1] * In2->M[1][2] + In1->M[0][2] * In2->M[2][2];

	Out->M[1][0] = In1->M[1][0] * In2->M[0][0] + In1->M[1][1] * In2->M[1][0] + In1->M[1][2] * In2->M[2][0];
	Out->M[1][1] = In1->M[1][0] * In2->M[0][1] + In1->M[1][1] * In2->M[1][1] + In1->M[1][2] * In2->M[2][1];
	Out->M[1][2] = In1->M[1][0] * In2->M[0][2] + In1->M[1][1] * In2->M[1][2] + In1->M[1][2] * In2->M[2][2];

	Out->M[2][0] = In1->M[2][0] * In2->M[0][0] + In1->M[2][1] * In2->M[1][0] + In1->M[2][2] * In2->M[2][0];
	Out->M[2][1] = In1->M[2][0] * In2->M[0][1] + In1->M[2][1] * In2->M[1][1] + In1->M[2][2] * In2->M[2][1];
	Out->M[2][2] = In1->M[2][0] * In2->M[0][2] + In1->M[2][1] * In2->M[1][2] + In1->M[2][2] * In2->M[2][2];
}
/////////////////////////////////////
// 角度制限を判定する共通関数 (subIndexJdgの判定は割りと不明…)
void CheckLimitAngle(
	const FVector& RotMin,
	const FVector& RotMax,
	FVector * outAngle, //target angle ( in and out param)
	bool subIndexJdg //(ik link index < ik loop temp):: linkBoneIndex < ikt
	)
{
//#define DEBUG_CheckLimitAngle
#ifdef DEBUG_CheckLimitAngle
	FVector debugVec = *outAngle;
#endif
#if 0
	if (outAngle->X < RotMin.X)
	{
		float tf = 2 * RotMin.X - outAngle->X;
		outAngle->X = tf <= RotMax.X && subIndexJdg ? tf : RotMin.X;
	}
	if (outAngle->X > RotMax.X)
	{
		float tf = 2 * RotMax.X - outAngle->X;
		outAngle->X = tf >= RotMin.X && subIndexJdg ? tf : RotMax.X;
	}
	if (outAngle->Y < RotMin.Y)
	{
		float tf = 2 * RotMin.Y - outAngle->Y;
		outAngle->Y = tf <= RotMax.Y && subIndexJdg ? tf : RotMin.Y;
	}
	if (outAngle->Y > RotMax.Y)
	{
		float tf = 2 * RotMax.Y - outAngle->Y;
		outAngle->Y = tf >= RotMin.Y && subIndexJdg ? tf : RotMax.Y;
	}
	if (outAngle->Z < RotMin.Z)
	{
		float tf = 2 * RotMin.Z - outAngle->Z;
		outAngle->Z = tf <= RotMax.Z && subIndexJdg ? tf : RotMin.Z;
	}
	if (outAngle->Z > RotMax.Z)
	{
		float tf = 2 * RotMax.Z - outAngle->Z;
		outAngle->Z = tf >= RotMin.Z && subIndexJdg ? tf : RotMax.Z;
	}
#else
	*outAngle = ClampVector(
		*outAngle,
		RotMin,
		RotMax
		);
#endif
	//debug
#ifdef DEBUG_CheckLimitAngle
	UE_LOG(LogMMD4UE4_VMDFactory, Log,
		TEXT("CheckLimitAngle::out[%s]<-In[%s]:MI[%s]MX[%s]"),
		*outAngle->ToString(),
		*debugVec.ToString(),
		*RotMin.ToString(),
		*RotMax.ToString()
		);
#endif
}
//////////////////////////////////////////////////////////////////////////////////////

/*****************
* MMD側の名称からTableRowのUE側名称を検索し取得する
* Return :T is Found
* @param :ue4Name is Found Row Name
****************/
bool UVmdFactory::FindTableRowMMD2UEName(
	UDataTable* ReNameTable,
	FName mmdName,
	FName * ue4Name
	)
{
	if (ReNameTable == NULL || ue4Name == NULL)
	{
		return false;
	}

	TArray<FName> getTableNames = ReNameTable->GetRowNames();

	FMMD2UE4NameTableRow* dataRow;
	FString ContextString;
	for (int i = 0; i < getTableNames.Num(); ++i)
	{
		ContextString = "";
		dataRow = ReNameTable->FindRow<FMMD2UE4NameTableRow>(getTableNames[i], ContextString);
		if (dataRow)
		{
			if (mmdName == FName(*dataRow->MmdOriginalName))
			{
				*ue4Name = getTableNames[i];
				return true;
			}
		}
	}
	return false;
}

/*****************
* Bone名称からRefSkeltonで一致するBoneIndexを検索し取得する
* Return :index, -1 is not found
* @param :TargetName is Target Bone Name
****************/
int32 UVmdFactory::FindRefBoneInfoIndexFromBoneName(
	const FReferenceSkeleton & RefSkelton,
	const FName & TargetName
	)
{
	for (int i = 0; i < RefSkelton.GetRefBoneInfo().Num(); ++i)
	{
		if (RefSkelton.GetRefBoneInfo()[i].Name == TargetName)
		{
			return i;
		}
	}
	return -1;
}


/*****************
* 現在のキーにおける指定BoneのGlb座標を再帰的に算出する
* Return :trncform
* @param :TargetName is Target Bone Name
****************/
FTransform UVmdFactory::CalcGlbTransformFromBoneIndex(
	UAnimSequence* DestSeq,
	USkeleton* Skeleton,
	int32 BoneIndex,
	int32 keyIndex
	)
{
	if (DestSeq == NULL || Skeleton == NULL || BoneIndex < 0 || keyIndex < 0)
	{
		//error root
		return FTransform::Identity;
	}

	auto& dat = DestSeq->GetDataModel()->GetBoneAnimationTracks()[BoneIndex].InternalTrackData;

	FTransform resultTrans(
		FQuat(dat.RotKeys[keyIndex]),// qt.X, qt.Y, qt.Z, qt.W),
		FVector(dat.PosKeys[keyIndex]),
		FVector(dat.ScaleKeys[keyIndex])
	);

	int ParentBoneIndex = Skeleton->GetReferenceSkeleton().GetParentIndex(BoneIndex);
	if (ParentBoneIndex >= 0)
	{
		//found parent bone
		resultTrans *= CalcGlbTransformFromBoneIndex(
			DestSeq,
			Skeleton,
			ParentBoneIndex,
			keyIndex
			);
	}
	return resultTrans;
}

#undef LOCTEXT_NAMESPACE
