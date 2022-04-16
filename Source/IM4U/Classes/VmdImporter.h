
// Copyright 2015-2017 BlackMa9. All Rights Reserved.
#pragma once


#include "Engine.h"
#include "MMDImportHelper.h"
#include "VmdImportUI.h"


// Copy From DxLib DxModelLoaderVMD.h
// DX Library Copyright (C) 2001-2008 Takumi Yamada.

//MMDのモーションデータ(VMD)形式　めも2　(表示・IK)
//http://blog.goo.ne.jp/torisu_tetosuki/e/2a2cb5c2de7563c5e6f20b19e1fe6139

//#define uint8 (unsigned char)

namespace MMD4UE4
{

	struct VMD_HEADER
	{
		char header[30];	// Vocaloid Motion Data 0002
		char modelName[20];	// Target Model Name
	};

#define D_VMD_KEY_BEZIER_AR_0_BEZ_0		0		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_0_BEZ_1		1		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_1_BEZ_X		0		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_1_BEZ_Y		1		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_2_KND_X		0		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_2_KND_Y		1		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_2_KND_Z		2		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
#define D_VMD_KEY_BEZIER_AR_2_KND_R		3		/* [AR0;BEZ-id][AR1:xy][AR1:XYZ R]**/
	// VMDキーデータ( 111byte )
	struct VMD_KEY
	{
		//uint8	Data[111];
		
		char	Name[ 15 ] ;						// 名前
		uint32	Frame ;								// フレーム
		float	Position[ 3 ] ;						// 座標
		float	Quaternion[ 4 ] ;					// クォータニオン
		/*float	PosXBezier[ 4 ] ;					// 座標Ｘ用ベジェ曲線情報
		float	PosYBezier[ 4 ] ;					// 座標Ｙ用ベジェ曲線情報
		float	PosZBezier[ 4 ] ;					// 座標Ｚ用ベジェ曲線情報
		float	RotBezier[4 ] ;					// 回転用ベジェ曲線情報
		*/
		uint8	Bezier[2][2][4];					// [id] [xy][XYZ R]

	};

	// VMD表情キーデータ( 23byte )
	struct VMD_FACE_KEY
	{
		/*uint8	Data[23];						// データ
		*/
		char	Name[ 15 ] ;						// 表情名
		uint32	Frame ;								// フレーム
		float	Factor ;							// ブレンド率
		
	};

#define D_VMD_CAMERA_INTERPOLATION_AR_0_X		0	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_0_Y		1	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_0_Z		2	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_0_ROT		3	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_0_LEN		4	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_0_VIEW	5	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_1_X		0	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_1_Y		1	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_2_X		0	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
#define D_VMD_CAMERA_INTERPOLATION_AR_2_Y		1	/*Interpolation[AR0-6:X,Y,Z,Rot,Len,View][AR1-2:X,Y][AR2-2:P1,P2]*/
	// VMDカメラキーデータ
	struct VMD_CAMERA
	{
		/*uint8	Data[61];						// データ
		*/
		uint32	Frame;								//  4:  0:フレーム番号
		float	Length;								//  8:  4: -(距離)
		float	Location[3];						// 20:  8:位置
		float	Rotate[3];							// 32: 20:オイラー角 // X軸は符号が反転しているので注意
		uint8	Interpolation[6][2][2];				// 56: 32:補間情報 // [6:X,Y,Z,Rot,Len,View][2:X,Y][2:P1,P2]
		uint32	ViewingAngle;						// 60: 56:向き
		uint8	Perspective;						// 61: 60:射影カメラかどうか 0:射影カメラ 1:正射影カメラ
		
	};
	// VMD 照明
	struct VMD_LIGHT
	{
		uint32	flameNo;
		float	RGB[3];
		float	Loc[3];
	};
	// VMDセルフシャドー
	struct VMD_SELFSHADOW{
		uint32	FlameNo;
		uint8	Mode;
		float	Distance;
	};
	////////////////////////////////////////////////////
	// mmd 7.40以降で作成されたVMDの場合以下の拡張データが入る
	// 参考：MMDのモーションデータ(VMD)形式　めも2　(表示・IK)
	///////////////////////////////////////////////////
	// 表示・IKデータ
	//int dataCount; // データ数
	//VMD_VISIBLE_IK_DATA data[dataCount];
	////////////
	// IKデータ用構造体
	typedef struct _VMD_IK_DATA
	{
		char ikBoneName[20]; // IKボーン名
		unsigned char ikEnabled; // IK有効 // 0:off 1:on
	} VMD_IK_DATA;
	// 表示・IKデータ用構造体
	typedef struct _VMD_VISIBLE_IK_DATA
	{
		int frameNo; // フレーム番号
		unsigned char visible; // 表示 // 0:off 1:on
		int ikCount; // IK数
		TArray<VMD_IK_DATA> ikData; // ikData[ikCount]; // IKデータリスト
	} VMD_VISIBLE_IK_DATA;
	////////////////////////////////////////////////////

	//VMD Motion Data for Read-
	struct VmdReadMotionData
	{
		VMD_HEADER				vmdHeader;
		//Bone
		int32					vmdKeyCount;
		TArray<VMD_KEY>			vmdKeyList;
		//Skin
		int32					vmdFaceCount;
		TArray<VMD_FACE_KEY>	vmdFaceList;
		//camera 
		int32					vmdCameraCount;
		TArray<VMD_CAMERA>		vmdCameraList;
		//Light
		int32					vmdLightCount;
		TArray<VMD_LIGHT>		vmdLightList;
		//Self Shadow
		int32					vmdSelfShadowCount;
		TArray<VMD_SELFSHADOW>	vmdSelfShadowList;
		//extend info, ik visible
		int32					vmdExtIKCount;
		TArray<VMD_VISIBLE_IK_DATA>	vmdExtIKList;
	};

	struct VmdKeyTrackList
	{
		// Track Name
		FString					TrackName;
		// min Frame
		uint32					minFrameCount;
		// max Frame
		uint32					maxFrameCount;
		// Sorted Key Frame Data List
		TArray<VMD_KEY>			keyList;
		// sort frame num
		TArray<int32>			sortIndexList;
	};
	struct VmdFaceTrackList
	{
		// Track Name
		FString					TrackName;
		// min Frame
		uint32					minFrameCount;
		// max Frame
		uint32					maxFrameCount;
		// Sorted Key Frame Data List
		TArray<VMD_FACE_KEY>	keyList;
		// sort frame num
		TArray<int32>			sortIndexList;
	};
	struct VmdCameraTrackList
	{
		// Track Name ( const MMDCAM)
		FString					TrackName;
		// min Frame
		uint32					minFrameCount;
		// max Frame
		uint32					maxFrameCount;
		// Sorted Key Frame Data List
		TArray<VMD_CAMERA>		keyList;
		// sort frame num
		TArray<int32>			sortIndexList;
	};
	//
	DECLARE_LOG_CATEGORY_EXTERN(LogMMD4UE4_VmdMotionInfo, Log, All)
	// Inport用 meta data 格納クラス
	class VmdMotionInfo : public MMDImportHelper
	{
	public:
		VmdMotionInfo();
		~VmdMotionInfo();

		///////////////////////////////////////
		bool VMDLoaderBinary(
			const uint8 *& Buffer,
			const uint8 * BufferEnd
			);

		///////////////////////////////////////
		bool ConvertVMDFromReadData(
			VmdReadMotionData * readData
			);


		//////////////////////////////////////////
		enum EVMDKEYFRAMETYPE
		{
			EVMD_KEYBONE,
			EVMD_KEYFACE,
			EVMD_KEYCAM 
		};
		//////////////////////////////////////////
		// 指定List内で該当するFrame名があればそのIndex値を返す。異常値=-1。
		int32 FindKeyTrackName(
			FString targetName,
			EVMDKEYFRAMETYPE listType);

		//////////////////////////////////////////

		// vmd Target Model Name
		FString					ModelName;
		// min Frame 
		uint32					minFrame;
		// max Frams
		uint32					maxFrame;

		//Keys
		TArray<VmdKeyTrackList>	keyBoneList;
		//Skins
		TArray<VmdFaceTrackList> keyFaceList;
		//camera
		TArray<VmdCameraTrackList> keyCameraList;
	};

}


///////////////////////////////////////////////////////////////////
//	Compy Refafct FBImporter.h
/////////////////////////////////////////////

struct VMDImportOptions
{
	// General options
	FVector ImportTranslation;
	FRotator ImportRotation;
	float ImportUniformScale;
	//EMMDNormalImportMethod NormalImportMethod;
	// Static Mesh options
	bool bCombineToSingle;
	//EVertexColorImportOptionMMD::Type VertexColorImportOption;
	FColor VertexOverrideColor;
	// Skeletal Mesh options
	bool bImportMorph;
	bool bImportAnimations;
	// Animation option
	USkeleton* SkeletonForAnimation;
	USkeletalMesh* SkeletalMeshForAnimation;
	//EFBXAnimationLengthImportType AnimationLengthImportType;
	struct FIntPoint AnimationRange;
	FString AnimationName;

	UAnimSequence* AnimSequenceAsset;
	UDataTable* MMD2UE4NameTableRow;
	UMMDExtendAsset* MmdExtendAsset;
};


VMDImportOptions* GetVMDImportOptions(
	class FVmdImporter* PmxImporter,
	UVmdImportUI* ImportUI,
	bool bShowOptionDialog,
	const FString& FullPath,
	bool& bOutOperationCanceled,
	bool& bOutImportAll,
	bool bIsObjFormat,
	bool bForceImportType = false,
	EVMDImportType ImportType = VMDIT_Animation);


void ApplyVMDImportUIToImportOptions(
	UVmdImportUI* ImportUI,
	VMDImportOptions& InOutImportOptions);

/************************/


/**
* Main VMD Importer class.
*/
class FVmdImporter
{
public:
	~FVmdImporter();
	/**
	* Returns the importer singleton. It will be created on the first request.
	*/
	IM4U_API static FVmdImporter* GetInstance();
	static void DeleteInstance();

	/**
	* Get the object of import options
	*
	* @return FBXImportOptions
	*/
	IM4U_API VMDImportOptions* GetImportOptions() const;

	VMDImportOptions* ImportOptions;
protected:
	static TSharedPtr<FVmdImporter> StaticInstance;

	FVmdImporter();
	/**
	* Clean up for destroy the Importer.
	*/
	void CleanUp();

};

