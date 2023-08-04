// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once


#include "Engine.h"

#include "MMDImportHelper.h"
#include "PmxImportUI.h"
#include "MMDExtendAsset.h"

#include "MMDStaticMeshImportData.h"


/////////////////////////////////////////////////
// Copy From DxLib DxModelLoader4.h
// DX Library Copyright (C) 2001-2008 Takumi Yamada.

//#define uint8 (unsigned char)

namespace MMD4UE4
{

	// マクロ定義 -----------------------------------

	#define PMX_MAX_IKLINKNUM		(64)			// 対応するIKリンクの最大数

	// データ型定義 ---------------------------------

	// PMXファイルの情報を格納する構造体
	struct PMX_BASEINFO
	{
		uint8	EncodeType;							// 文字コードのエンコードタイプ 0:UTF16 1:UTF8
		uint8	UVNum;								// 追加ＵＶ数 ( 0～4 )
		uint8	VertexIndexSize;					// 頂点Indexサイズ ( 1 or 2 or 4 )
		uint8	TextureIndexSize;					// テクスチャIndexサイズ ( 1 or 2 or 4 )
		uint8	MaterialIndexSize;					// マテリアルIndexサイズ ( 1 or 2 or 4 )
		uint8	BoneIndexSize;						// ボーンIndexサイズ ( 1 or 2 or 4 )
		uint8	MorphIndexSize;						// モーフIndexサイズ ( 1 or 2 or 4 )
		uint8	RigidIndexSize;						// 剛体Indexサイズ ( 1 or 2 or 4 )
	};

	// 頂点データ
	struct PMX_VERTEX
	{
		FVector3f		Position;						// 座標
		FVector3f		Normal;							// 法線
		FVector2f	UV;								// 標準UV値
		FVector4f	AddUV[4];						// 追加UV値
		uint8		WeightType;						// ウエイトタイプ( 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF )
		uint32		BoneIndex[4];					// ボーンインデックス
		float		BoneWeight[4];					// ボーンウェイト
		FVector3f		SDEF_C;							// SDEF-C
		FVector3f		SDEF_R0;						// SDEF-R0
		FVector3f		SDEF_R1;						// SDEF-R1
		float		ToonEdgeScale;					// トゥーンエッジのスケール
	};

	// 面リスト
	struct PMX_FACE
	{
		uint32		VertexIndex[3];					//頂点IndexList
	};

	// テクスチャ情報
	struct PMX_TEXTURE
	{
		FString	TexturePath;
	};

	// マテリアル情報
	struct PMX_MATERIAL
	{
		FString	Name;						// 名前
		FString	NameEng;						// 名前

		float	Diffuse[4];						// ディフューズカラー
		float	Specular[3];						// スペキュラカラー
		float	SpecularPower;						// スペキュラ定数
		float	Ambient[3];						// アンビエントカラー

		uint8	CullingOff;						// 両面描画
		uint8	GroundShadow;						// 地面影
		uint8	SelfShadowMap;						// セルフシャドウマップへの描画
		uint8	SelfShadowDraw;					// セルフシャドウの描画
		uint8	EdgeDraw;							// エッジの描画

		float	EdgeColor[4];					// エッジカラー
		float	EdgeSize;							// エッジサイズ

		int		TextureIndex;						// 通常テクスチャインデックス
		int		SphereTextureIndex;				// スフィアテクスチャインデックス
		uint8	SphereMode;						// スフィアモード( 0:無効 1:乗算 2:加算 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)

		uint8	ToonFlag;							// 共有トゥーンフラグ( 0:個別Toon 1:共有Toon )
		int		ToonTextureIndex;					// トゥーンテクスチャインデックス

		int		MaterialFaceVerticeNum;					// マテリアルが適応されている面の数
	};

	// ＩＫリンク情報
	struct PMX_IKLINK
	{
		int		BoneIndex;							// リンクボーンのインデックス
		uint8	RotLockFlag;						// 回転制限( 0:OFF 1:ON )
		float	RotLockMin[3];					// 回転制限、下限
		float	RotLockMax[3];					// 回転制限、上限
	};

	// ＩＫ情報
	struct PMX_IK
	{
		int		TargetBoneIndex;					// IKターゲットのボーンインデックス
		int		LoopNum;							// IK計算のループ回数
		float	RotLimit;							// 計算一回辺りの制限角度

		int		LinkNum;							// ＩＫリンクの数
		PMX_IKLINK Link[PMX_MAX_IKLINKNUM];		// ＩＫリンク情報
	};

	// ボーン情報
	struct PMX_BONE
	{
		FString	Name;						// 名前
		FString	NameEng;						// 名前
		FVector3f	Position;						// 座標
		int		ParentBoneIndex;					// 親ボーンインデックス
		int		TransformLayer;					// 変形階層

		uint8	Flag_LinkDest;						// 接続先
		uint8	Flag_EnableRot;					// 回転ができるか
		uint8	Flag_EnableMov;					// 移動ができるか
		uint8	Flag_Disp;							// 表示
		uint8	Flag_EnableControl;				// 操作ができるか
		uint8	Flag_IK;							// IK
		uint8	Flag_AddRot;						// 回転付与
		uint8	Flag_AddMov;						// 移動付与
		uint8	Flag_LockAxis;						// 軸固定
		uint8	Flag_LocalAxis;					// ローカル軸
		uint8	Flag_AfterPhysicsTransform;		// 物理後変形
		uint8	Flag_OutParentTransform;			// 外部親変形

		FVector3f	OffsetPosition;				// オフセット座標
		int		LinkBoneIndex;						// 接続先ボーンインデックス
		int		AddParentBoneIndex;				// 付与親ボーンインデックス
		float	AddRatio;							// 付与率
		FVector3f	LockAxisVector;				// 軸固定時の軸の方向ベクトル
		FVector3f	LocalAxisXVector;				// ローカル軸のＸ軸
		FVector3f	LocalAxisZVector;				// ローカル軸のＺ軸
		int		OutParentTransformKey;				// 外部親変形時の Key値

		PMX_IK	IKInfo;							// ＩＫ情報

		FTransform3f absTF;
	};

	// 頂点モーフ情報
	struct PMX_MORPH_VERTEX
	{
		int		Index;								// 頂点インデックス
		FVector3f	Offset;						// 頂点座標オフセット
	};

	// ＵＶモーフ情報
	struct PMX_MORPH_UV
	{
		int		Index;								// 頂点インデックス
		float	Offset[4];							// 頂点ＵＶオフセット
	};

	// ボーンモーフ情報
	struct PMX_MORPH_BONE
	{
		int		Index;								// ボーンインデックス
		FVector3f	Offset;						// 座標オフセット
		float	Quat[4];							// 回転クォータニオン
	};

	// 材質モーフ情報
	struct PMX_MORPH_MATERIAL
	{
		int		Index;								// マテリアルインデックス
		uint8	CalcType;							// 計算タイプ( 0:乗算  1:加算 )
		float	Diffuse[4];						// ディフューズカラー
		float	Specular[3];						// スペキュラカラー
		float	SpecularPower;						// スペキュラ係数
		float	Ambient[3];						// アンビエントカラー
		float	EdgeColor[4];					// エッジカラー
		float	EdgeSize;							// エッジサイズ
		float	TextureScale[4];					// テクスチャ係数
		float	SphereTextureScale[4];			// スフィアテクスチャ係数
		float	ToonTextureScale[4];				// トゥーンテクスチャ係数
	};

	// グループモーフ
	struct PMX_MORPH_GROUP
	{
		int		Index;								// モーフインデックス
		float	Ratio;								// モーフ率
	};

	// モーフ情報
	struct PMX_MORPH
	{
		FString	Name;						// 名前
		FString	NameEng;						// 名前

		uint8	ControlPanel;						// 操作パネル
		uint8	Type;								// モーフの種類  0:グループ 1:頂点 2:ボーン 3:UV 4:追加UV1 5:追加UV2 6:追加UV3 7:追加UV4 8:材質

		int		DataNum;							// モーフ情報の数

		TArray<PMX_MORPH_VERTEX>	Vertex;				// 頂点モーフ
		TArray<PMX_MORPH_UV>		UV;						// UVモーフ
		TArray<PMX_MORPH_BONE>		Bone;					// ボーンモーフ
		TArray<PMX_MORPH_MATERIAL>	Material;			// マテリアルモーフ
		TArray<PMX_MORPH_GROUP>		Group;				// グループモーフ
	};

	// 剛体情報
	struct PMX_RIGIDBODY
	{
		FString	Name;						// 名前
		FName fnName;
		FString	NameEng;
		uint32	BoneIndex;							// 対象ボーン番号

		uint8	group;				// 剛体グループ番号
		uint16	collisionGroup;				// 剛体グループ対象

		uint8	ShapeType;							// 形状( 0:球  1:箱  2:カプセル )
		//float	ShapeW;							// 幅
		//float	ShapeH;							// 高さ
		//float	ShapeD;							// 奥行
		FVector3f  Size;

		FVector3f	Position;						// 位置
		FVector3f	Rotation;						// 回転( ラジアン )

		float	Mass;					// 質量
		float	PosDim;					// 移動減衰
		float	RotDim;					// 回転減衰
		float	Recoil;					// 反発力
		float	Friction;					// 摩擦力

		uint8	OpType;						// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )

		FQuat   rttQuat;
		int idx;
	};

	// ジョイント情報
	struct PMX_JOINT
	{
		FString	Name;						// 名前
		FString	NameEng;
		uint8	Type;								// 種類( 0:スプリング6DOF ( PMX2.0 では 0 のみ )

		int		RigidBodyAIndex;					// 接続先剛体Ａ
		int		RigidBodyBIndex;					// 接続先剛体Ｂ

		FVector3f	Position;						// 位置
		FVector3f	Rotation;						// 回転( ラジアン )

		FVector3f	ConstrainPositionMin;			// 移動制限-下限
		FVector3f	ConstrainPositionMax;			// 移動制限-上限
		FVector3f	ConstrainRotationMin;			// 回転制限-下限
		FVector3f	ConstrainRotationMax;			// 回転制限-上限

		FVector3f	SpringPosition;				// バネ定数-移動
		FVector3f	SpringRotation;				// バネ定数-回転

		FQuat       Quat;
	};
	//////////////////////////////////////////////////////////////

	struct PMX_BONE_HIERARCHY
	{
		int		originalBoneIndex;
		int		fixedBoneIndex;
		bool	fixFlag_Parent;
		//bool	fixFlag_Target;
	};
	//////////////////////////////////////////////////////////////

	DECLARE_LOG_CATEGORY_EXTERN(LogMMD4UE4_PmxMeshInfo, Log, All)

	////////////////////////////////////////////////////////////////////
	// Inport用 meta data 格納クラス
	class PmxMeshInfo : public MMDImportHelper
	{
		//////////////////////////////////////
		// Sort Parent Bone ( sort tree bones)
		// memo: ボーンの配列で子->親の順の場合、
		//       ProcessImportMeshSkeleton内部のcheckに引っかかりクラッシュするため。
		// how to: after PMXLoaderBinary func.
		//////////////////////////////////////
		bool FixSortParentBoneIndex();

		TArray<PMX_BONE_HIERARCHY>	fixedHierarchyBone;

	public:
		PmxMeshInfo();
		~PmxMeshInfo();

		///////////////////////////////////////
		bool PMXLoaderBinary(
			const uint8 *& Buffer,
			const uint8 * BufferEnd
			);
		template <typename T>
		void readBuffer(T& var) {
			size_t memcopySize = sizeof(var);
			var = *(T*)Buffer;
			//FMemory::Memcpy(&var, Buffer, memcopySize);
			Buffer += memcopySize;
		}

		TArray<PMX_RIGIDBODY> findRigids(FName bname) {
			TArray<PMX_RIGIDBODY> list;
			for (int i = 0; i < rigidList.Num(); i++)
				if (rigidList[i].fnName == bname)
					list.Add(rigidList[i]);
			return list;
		}
		PMX_RIGIDBODY *findRigid(FName bname) {
			
			for (int i = 0; i < rigidList.Num(); i++)
				if (rigidList[i].fnName == bname)
					return &rigidList[i];
			return nullptr;
		}
		PMX_JOINT* findJoint(PMX_RIGIDBODY* rb1, PMX_RIGIDBODY* rb2);
		///////////////////////////////////////
		const uint8* Buffer;
		char				magic[4];
		float				formatVer;
		PMX_BASEINFO		baseHeader;
		FString				modelNameJP;
		FString				modelNameEng;
		FString				modelCommentJP;
		FString				modelCommentEng;
		//
		TArray<PMX_VERTEX>	vertexList;
		TArray<PMX_FACE>	faseList;

		TArray<PMX_TEXTURE>	textureList;
		TArray<PMX_MATERIAL>	materialList;

		TArray<PMX_BONE>	boneList;
		TArray<PMX_MORPH>	morphList;
		TArray<PMX_RIGIDBODY>	rigidList;
		TArray<PMX_JOINT> jointList;

	};

}

///////////////////////////////////////////////////////////////////
//	Compy Refafct FBImporter.h
/////////////////////////////////////////////

struct PMXImportOptions
{
	// General options
	bool bImportMaterials;
	bool bInvertNormalMap;
	bool bImportTextures;
	bool bCreateMaterialInstMode;
	bool bUnlitMaterials;
	bool bImportLOD;
	FString MeshName;
	bool bUsedAsFullName;
	bool bConvertScene;
	bool bRemoveNameSpace;
	FVector ImportTranslation;
	FRotator ImportRotation;
	float ImportUniformScale;
	EMMDNormalImportMethod NormalImportMethod;
	// Static Mesh options
	bool bCombineToSingle;
	EVertexColorImportOptionMMD::Type VertexColorImportOption;
	FColor VertexOverrideColor;
	bool bRemoveDegenerates;
	bool bGenerateLightmapUVs;
	bool bOneConvexHullPerUCX;
	bool bAutoGenerateCollision;
	FName StaticMeshLODGroup;
	// Skeletal Mesh options
	bool bImportMorph;
	bool bImportAnimations;
	bool bUpdateSkeletonReferencePose;
	bool bResample;
	bool bImportRigidMesh;
	bool bUseT0AsRefPose;
	bool bPreserveSmoothingGroups;
	bool bKeepOverlappingVertices;
	bool bImportMeshesInBoneHierarchy;
	bool bCreatePhysicsAsset;
	UPhysicsAsset *PhysicsAsset;
	// Animation option
	USkeleton* SkeletonForAnimation;
	//EFBXAnimationLengthImportType AnimationLengthImportType;
	FIntPoint AnimationRange;
	FString AnimationName;
	bool	bPreserveLocalTransform;
	bool	bImportCustomAttribute;

	/*bool ShouldImportNormals()
	{
	return NormalImportMethod == FBXNIM_ImportNormals || NormalImportMethod == FBXNIM_ImportNormalsAndTangents;
	}

	bool ShouldImportTangents()
	{
	return NormalImportMethod == FBXNIM_ImportNormalsAndTangents;
	}

	void ResetForReimportAnimation()
	{
	bImportMorph = true;
	AnimationLengthImportType = FBXALIT_ExportedTime;
	}*/
	UAnimSequence* AnimSequenceAsset;
	UDataTable* MMD2UE4NameTableRow;
	UMMDExtendAsset* MmdExtendAsset;
};

PMXImportOptions* GetImportOptions(
	class FPmxImporter* PmxImporter,
	UPmxImportUI* ImportUI,
	bool bShowOptionDialog,
	const FString& FullPath,
	bool& bOutOperationCanceled,
	bool& bOutImportAll,
	bool& skipModel,
	bool bIsObjFormat,
	bool bForceImportType = false,
	EPMXImportType ImportType = PMXIT_StaticMesh
	);

void ApplyImportUIToImportOptions(
	UPmxImportUI* ImportUI,
	PMXImportOptions& InOutImportOptions
	);

/**
* Main PMX Importer class.
*/
class FPmxImporter
{
public:
	~FPmxImporter();
	/**
	* Returns the importer singleton. It will be created on the first request.
	*/
	IM4U_API static FPmxImporter* GetInstance();
	static void DeleteInstance();

#if 0
	/**
	* Detect if the FBX file has skeletal mesh model. If there is deformer definition, then there is skeletal mesh.
	* In this function, we don't need to import the scene. But the open process is time-consume if the file is large.
	*
	* @param InFilename	FBX file name.
	* @return int32 -1 if parse failed; 0 if geometry ; 1 if there are deformers; 2 otherwise
	*/
	int32 GetImportType(const FString& InFilename);

	/**
	* Get detail infomation in the Fbx scene
	*
	* @param Filename Fbx file name
	* @param SceneInfo return the scene info
	* @return bool true if get scene info successfully
	*/
	bool GetSceneInfo(FString Filename, FbxSceneInfo& SceneInfo);


	/**
	* Initialize Fbx file for import.
	*
	* @param Filename
	* @param bParseStatistics
	* @return bool
	*/
	bool OpenFile(FString Filename, bool bParseStatistics, bool bForSceneInfo = false);

	/**
	* Import Fbx file.
	*
	* @param Filename
	* @return bool
	*/
	bool ImportFile(FString Filename);

	/**
	* Attempt to load an FBX scene from a given filename.
	*
	* @param Filename FBX file name to import.
	* @returns true on success.
	*/
	UNREALED_API bool ImportFromFile(const FString& Filename, const FString& Type);

	/**
	* Retrieve the FBX loader's error message explaining its failure to read a given FBX file.
	* Note that the message should be valid even if the parser is successful and may contain warnings.
	*
	* @ return TCHAR*	the error message
	*/
	const TCHAR* GetErrorMessage() const
	{
		return *ErrorMessage;
	}

	/**
	* Retrieve the object inside the FBX scene from the name
	*
	* @param ObjectName	Fbx object name
	* @param Root	Root node, retrieve from it
	* @return FbxNode*	Fbx object node
	*/
	FbxNode* RetrieveObjectFromName(const TCHAR* ObjectName, FbxNode* Root = NULL);

	/**
	* Creates a static mesh with the given name and flags, imported from within the FBX scene.
	* @param InParent
	* @param Node	Fbx Node to import
	* @param Name	the Unreal Mesh name after import
	* @param Flags
	* @param InStaticMesh	if LODIndex is not 0, this is the base mesh object. otherwise is NULL
	* @param LODIndex	 LOD level to import to
	*
	* @returns UObject*	the UStaticMesh object.
	*/
	UNREALED_API UStaticMesh* ImportStaticMesh(UObject* InParent, FbxNode* Node, const FName& Name, EObjectFlags Flags, UFbxStaticMeshImportData* ImportData, UStaticMesh* InStaticMesh = NULL, int LODIndex = 0);

	/**
	* Creates a static mesh from all the meshes in FBX scene with the given name and flags.
	*
	* @param InParent
	* @param MeshNodeArray	Fbx Nodes to import
	* @param Name	the Unreal Mesh name after import
	* @param Flags
	* @param InStaticMesh	if LODIndex is not 0, this is the base mesh object. otherwise is NULL
	* @param LODIndex	 LOD level to import to
	*
	* @returns UObject*	the UStaticMesh object.
	*/
	UNREALED_API UStaticMesh* ImportStaticMeshAsSingle(UObject* InParent, TArray<FbxNode*>& MeshNodeArray, const FName InName, EObjectFlags Flags, UFbxStaticMeshImportData* TemplateImportData, UStaticMesh* InStaticMesh, int LODIndex = 0);

	void ImportStaticMeshSockets(UStaticMesh* StaticMesh);

	/**
	* re-import Unreal static mesh from updated Fbx file
	* if the Fbx mesh is in LODGroup, the LOD of mesh will be updated
	*
	* @param Mesh the original Unreal static mesh object
	* @return UObject* the new Unreal mesh object
	*/
	UStaticMesh* ReimportStaticMesh(UStaticMesh* Mesh, UFbxStaticMeshImportData* TemplateImportData);

	/**
	* re-import Unreal skeletal mesh from updated Fbx file
	* If the Fbx mesh is in LODGroup, the LOD of mesh will be updated.
	* If the FBX mesh contains morph, the morph is updated.
	* Materials, textures and animation attached in the FBX mesh will not be updated.
	*
	* @param Mesh the original Unreal skeletal mesh object
	* @return UObject* the new Unreal mesh object
	*/
	USkeletalMesh* ReimportSkeletalMesh(USkeletalMesh* Mesh, UFbxSkeletalMeshImportData* TemplateImportData);

	/**
	* Creates a skeletal mesh from Fbx Nodes with the given name and flags, imported from within the FBX scene.
	* These Fbx Nodes bind to same skeleton. We need to bind them to one skeletal mesh.
	*
	* @param InParent
	* @param NodeArray	Fbx Nodes to import
	* @param Name	the Unreal Mesh name after import
	* @param Flags
	* @param Filename	Fbx file name
	* @param FbxShapeArray	Fbx Morph objects.
	* @param OutData - Optional import data to populate
	* @param bCreateRenderData - Whether or not skeletal mesh rendering data will be created.
	*
	* @return The USkeletalMesh object created
	*/
	USkeletalMesh* ImportSkeletalMesh(UObject* InParent, TArray<FbxNode*>& NodeArray, const FName& Name, EObjectFlags Flags, UFbxSkeletalMeshImportData* TemplateImportData, FString Filename, TArray<FbxShape*> *FbxShapeArray = NULL, FSkeletalMeshImportData* OutData = NULL, bool bCreateRenderData = true);

	/**
	* Add to the animation set, the animations contained within the FBX scene, for the given skeletal mesh
	*
	* @param Skeleton	Skeleton that the animation belong to
	* @param SortedLinks	skeleton nodes which are sorted
	* @param Filename	Fbx file name
	* @param NodeArray node array of FBX meshes
	*/
	UAnimSequence* ImportAnimations(USkeleton* Skeleton, UObject* Outer, TArray<FbxNode*>& SortedLinks, const FString& Name, UFbxAnimSequenceImportData* TemplateImportData, TArray<FbxNode*>& NodeArray);

	/**
	* Get Animation Time Span - duration of the animation
	*/
	FbxTimeSpan GetAnimationTimeSpan(FbxNode* RootNode, FbxAnimStack* AnimStack);

	/**
	* Import one animation from CurAnimStack
	*
	* @param Skeleton	Skeleton that the animation belong to
	* @param DestSeq 	Sequence it's overwriting data to
	* @param Filename	Fbx file name	(not whole path)
	* @param SortedLinks	skeleton nodes which are sorted
	* @param NodeArray node array of FBX meshes
	* @param CurAnimStack 	Animation Data
	* @param ResampleRate	Resample Rate for data
	* @param AnimTimeSpan	AnimTimeSpan
	*/
	bool ImportAnimation(USkeleton* Skeleton, UAnimSequence* DestSeq, const FString& FileName, TArray<FbxNode*>& SortedLinks, TArray<FbxNode*>& NodeArray, FbxAnimStack* CurAnimStack, const int32 ResampleRate, const FbxTimeSpan AnimTimeSpan);
	/**
	* Calculate Max Sample Rate - separate out of the original ImportAnimations
	*
	* @param SortedLinks	skeleton nodes which are sorted
	* @param NodeArray node array of FBX meshes
	*/
	int32 GetMaxSampleRate(TArray<FbxNode*>& SortedLinks, TArray<FbxNode*>& NodeArray);
	/**
	* Validate Anim Stack - multiple check for validating animstack
	*
	* @param SortedLinks	skeleton nodes which are sorted
	* @param NodeArray node array of FBX meshes
	* @param CurAnimStack 	Animation Data
	* @param ResampleRate	Resample Rate for data
	* @param AnimTimeSpan	AnimTimeSpan
	*/
	bool ValidateAnimStack(TArray<FbxNode*>& SortedLinks, TArray<FbxNode*>& NodeArray, FbxAnimStack* CurAnimStack, int32 ResampleRate, bool bImportMorph, FbxTimeSpan &AnimTimeSpan);

	/**
	* Import Fbx Morph object for the Skeletal Mesh.
	* In Fbx, morph object is a property of the Fbx Node.
	*
	* @param SkelMeshNodeArray - Fbx Nodes that the base Skeletal Mesh construct from
	* @param BaseSkelMesh - base Skeletal Mesh
	* @param Filename - Fbx file name
	* @param LODIndex - LOD index
	*/
	void ImportFbxMorphTarget(TArray<FbxNode*> &SkelMeshNodeArray, USkeletalMesh* BaseSkelMesh, UObject* Parent, const FString& Filename, int32 LODIndex, UMMDSkeletalMeshImportData &dat);

	/**
	* Import LOD object for skeletal mesh
	*
	* @param InSkeletalMesh - LOD mesh object
	* @param BaseSkeletalMesh - base mesh object
	* @param DesiredLOD - LOD level
	* @param bNeedToReregister - if true, re-register this skeletal mesh to shut down the skeletal mesh component that is previewing this mesh.
	But you can set this to false when in the first loading before rendering this mesh for a performance issue
	@param ReregisterAssociatedComponents - if NULL, just re-registers all SkinnedMeshComponents but if you set the specific components, will only re-registers those components
	*/
	bool ImportSkeletalMeshLOD(USkeletalMesh* InSkeletalMesh, USkeletalMesh* BaseSkeletalMesh, int32 DesiredLOD, bool bNeedToReregister = true, TArray<UActorComponent*>* ReregisterAssociatedComponents = NULL);

	/**
	* Empties the FBX scene, releasing its memory.
	* Currently, we can't release KFbxSdkManager because Fbx Sdk2010.2 has a bug that FBX can only has one global sdkmanager.
	* From Fbx Sdk2011, we can create multiple KFbxSdkManager, then we can release it.
	*/
	UNREALED_API void ReleaseScene();

	/**
	* If the node model is a collision model, then fill it into collision model list
	*
	* @param Node Fbx node
	* @return true if the node is a collision model
	*/
	bool FillCollisionModelList(FbxNode* Node);

	/**
	* Import collision models for one static mesh if it has collision models
	*
	* @param StaticMesh - mesh object to import collision models
	* @param NodeName - name of Fbx node that the static mesh constructed from
	* @return return true if the static mesh has collision model and import successfully
	*/
	bool ImportCollisionModels(UStaticMesh* StaticMesh, const FbxString& NodeName);

	//help
	ANSICHAR* MakeName(const ANSICHAR* name);
	FName MakeNameForMesh(FString InName, FbxObject* FbxObject);

	// meshes

	/**
	* Get all Fbx skeletal mesh objects in the scene. these meshes are grouped by skeleton they bind to
	*
	* @param Node Root node to find skeletal meshes
	* @param outSkelMeshArray return Fbx meshes they are grouped by skeleton
	*/
	void FillFbxSkelMeshArrayInScene(FbxNode* Node, TArray< TArray<FbxNode*>* >& outSkelMeshArray, bool ExpandLOD);

	/**
	* Find FBX meshes that match Unreal skeletal mesh according to the bone of mesh
	*
	* @param FillInMesh     Unreal skeletal mesh
	* @param bExpandLOD     flag that if expand FBX LOD group when get the FBX node
	* @param OutFBXMeshNodeArray  return FBX mesh nodes that match the Unreal skeletal mesh
	*
	* @return the root bone that bind to the FBX skeletal meshes
	*/
	FbxNode* FindFBXMeshesByBone(const FName& RootBoneName, bool bExpandLOD, TArray<FbxNode*>& OutFBXMeshNodeArray);

	/**
	* Get mesh count (including static mesh and skeletal mesh, except collision models) and find collision models
	*
	* @param Node			Root node to find meshes
	* @param bCountLODs		Whether or not to count meshes in LOD groups
	* @return int32 mesh count
	*/
	int32 GetFbxMeshCount(FbxNode* Node, bool bCountLODs, int32 &OutNumLODGroups);

	/**
	* Get all Fbx mesh objects
	*
	* @param Node Root node to find meshes
	* @param outMeshArray return Fbx meshes
	*/
	UNREALED_API void FillFbxMeshArray(FbxNode* Node, TArray<FbxNode*>& outMeshArray, UnFbx::FFbxImporter* FFbxImporter);

	/**
	* Fill FBX skeletons to OutSortedLinks recursively
	*
	* @param Link Fbx node of skeleton root
	* @param OutSortedLinks
	*/
	void RecursiveBuildSkeleton(FbxNode* Link, TArray<FbxNode*>& OutSortedLinks);

	/**
	* Fill FBX skeletons to OutSortedLinks
	*
	* @param ClusterArray Fbx clusters of FBX skeletal meshes
	* @param OutSortedLinks
	*/
	void BuildSkeletonSystem(TArray<FbxCluster*>& ClusterArray, TArray<FbxNode*>& OutSortedLinks);

	/**
	* Get Unreal skeleton root from the FBX skeleton node.
	* Mesh and dummy can be used as skeleton.
	*
	* @param Link one FBX skeleton node
	*/
	FbxNode* GetRootSkeleton(FbxNode* Link);
#endif
	/**
	* Get the object of import options
	*
	* @return FBXImportOptions
	*/
	IM4U_API PMXImportOptions* GetImportOptions() const;
#if 0
	/** helper function **/
	UNREALED_API static void DumpFBXNode(FbxNode* Node);

	/**
	* Apply asset import settings for transform to an FBX node
	*
	* @param Node Node to apply transform settings too
	* @param AssetData the asset data object to get transform data from
	*/
	void ApplyTransformSettingsToFbxNode(FbxNode* Node, UFbxAssetImportData* AssetData);

	/**
	* Remove asset import settings for transform to an FBX node
	*
	* @param Node Node to apply transform settings too
	* @param AssetData the asset data object to get transform data from
	*/
	void RemoveTransformSettingsFromFbxNode(FbxNode* Node, UFbxAssetImportData* AssetData);

	/**
	* Populate the given matrix with the correct information for the asset data, in
	* a format that matches FBX internals or without conversion
	*
	* @param OutMatrix The matrix to fill
	* @param AssetData The asset data to extract the transform info from
	*/
	void BuildFbxMatrixForImportTransform(FbxAMatrix& OutMatrix, UFbxAssetImportData* AssetData);

private:
	/**
	* ActorX plug-in can export mesh and dummy as skeleton.
	* For the mesh and dummy in the skeleton hierarchy, convert them to FBX skeleton.
	*
	* @param Node          root skeleton node
	* @param SkelMeshes    skeletal meshes that bind to this skeleton
	* @param bImportNestedMeshes	if true we will import meshes nested in bone hierarchies instead of converting them to bones
	*/
	void RecursiveFixSkeleton(FbxNode* Node, TArray<FbxNode*> &SkelMeshes, bool bImportNestedMeshes);

	/**
	* Get all Fbx skeletal mesh objects which are grouped by skeleton they bind to
	*
	* @param Node Root node to find skeletal meshes
	* @param outSkelMeshArray return Fbx meshes they are grouped by skeleton
	* @param SkeletonArray
	* @param ExpandLOD flag of expanding LOD to get each mesh
	*/
	void RecursiveFindFbxSkelMesh(FbxNode* Node, TArray< TArray<FbxNode*>* >& outSkelMeshArray, TArray<FbxNode*>& SkeletonArray, bool ExpandLOD);

	/**
	* Get all Fbx rigid mesh objects which are grouped by skeleton hierarchy
	*
	* @param Node Root node to find skeletal meshes
	* @param outSkelMeshArray return Fbx meshes they are grouped by skeleton hierarchy
	* @param SkeletonArray
	* @param ExpandLOD flag of expanding LOD to get each mesh
	*/
	void RecursiveFindRigidMesh(FbxNode* Node, TArray< TArray<FbxNode*>* >& outSkelMeshArray, TArray<FbxNode*>& SkeletonArray, bool ExpandLOD);

	/**
	* Import Fbx Morph object for the Skeletal Mesh.  Each morph target import processing occurs in a different thread
	*
	* @param SkelMeshNodeArray - Fbx Nodes that the base Skeletal Mesh construct from
	* @param BaseSkelMesh - base Skeletal Mesh
	* @param Filename - Fbx file name
	* @param LODIndex - LOD index of the skeletal mesh
	*/
	void ImportMorphTargetsInternal(TArray<FbxNode*>& SkelMeshNodeArray, USkeletalMesh* BaseSkelMesh, UObject* Parent, const FString& InFilename, int32 LODIndex);

	/**
	* sub-method called from ImportSkeletalMeshLOD method
	*
	* @param InSkeletalMesh - newly created mesh used as LOD
	* @param BaseSkeletalMesh - the destination mesh object
	* @param DesiredLOD - the LOD index to import into. A new LOD entry is created if one doesn't exist
	*/
	void InsertNewLODToBaseSkeletalMesh(USkeletalMesh* InSkeletalMesh, USkeletalMesh* BaseSkeletalMesh, int32 DesiredLOD);

	/**
	* Method used to verify if the geometry is valid. For example, if the bounding box is tiny we should warn
	* @param StaticMesh - The imported static mesh which we'd like to verify
	*/
	void VerifyGeometry(UStaticMesh* StaticMesh);

public:
	// current Fbx scene we are importing. Make sure to release it after import
	FbxScene* Scene;
#endif
public:
	PMXImportOptions* ImportOptions;
protected:
#if 0
	enum IMPORTPHASE
	{
		NOTSTARTED,
		FILEOPENED,
		IMPORTED
	};
#endif
	static TSharedPtr<FPmxImporter> StaticInstance;
#if 0
	struct FFbxMaterial
	{
		FbxSurfaceMaterial* FbxMaterial;
		UMaterialInterface* Material;

		FString GetName() const { return FbxMaterial ? ANSI_TO_TCHAR(FbxMaterial->GetName()) : TEXT("None"); }
	};

	// scene management
	FFbxDataConverter Converter;
	FbxGeometryConverter* GeometryConverter;
	FbxManager* SdkManager;
	FbxImporter* Importer;
	IMPORTPHASE CurPhase;
	FString ErrorMessage;
	// base path of fbx file
	FString FileBasePath;
	TWeakObjectPtr<UObject> Parent;
	// Flag that the mesh is the first mesh to import in current FBX scene
	// FBX scene may contain multiple meshes, importer can import them at one time.
	// Initialized as true when start to import a FBX scene
	bool bFirstMesh;

	/**
	* Collision model list. The key is fbx node name
	* If there is an collision model with old name format, the key is empty string("").
	*/
	FbxMap<FbxString, TSharedPtr< FbxArray<FbxNode* > > > CollisionModels;
#endif
	FPmxImporter();
#if 0

	/**
	* Set up the static mesh data from Fbx Mesh.
	*
	* @param FbxMesh  Fbx Mesh object
	* @param StaticMesh Unreal static mesh object to fill data into
	* @param LODIndex	LOD level to set up for StaticMesh
	* @return bool true if set up successfully
	*/
	bool BuildStaticMeshFromGeometry(FbxMesh* FbxMesh, UStaticMesh* StaticMesh, TArray<FFbxMaterial>& MeshMaterials, int LODIndex,
		EVertexColorImportOption::Type VertexColorImportOption, const TMap<FVector3f, FColor>& ExistingVertexColorData, const FColor& VertexOverrideColor);
#endif
	/**
	* Clean up for destroy the Importer.
	*/
	void CleanUp();
#if 0
	/**
	* Compute the global matrix for Fbx Node
	*
	* @param Node	Fbx Node
	* @return KFbxXMatrix*	The global transform matrix
	*/
	FbxAMatrix ComputeTotalMatrix(FbxNode* Node);

	/**
	* Check if there are negative scale in the transform matrix and its number is odd.
	* @return bool True if there are negative scale and its number is 1 or 3.
	*/
	bool IsOddNegativeScale(FbxAMatrix& TotalMatrix);

	// various actors, current the Fbx importer don't importe them
	/**
	* Import Fbx light
	*
	* @param FbxLight fbx light object
	* @param World in which to create the light
	* @return ALight*
	*/
	ALight* CreateLight(FbxLight* InLight, UWorld* InWorld);
	/**
	* Import Light detail info
	*
	* @param FbxLight
	* @param UnrealLight
	* @return  bool
	*/
	bool FillLightComponent(FbxLight* Light, ULightComponent* UnrealLight);
	/**
	* Import Fbx Camera object
	*
	* @param FbxCamera Fbx camera object
	* @param World in which to create the camera
	* @return ACameraActor*
	*/
	ACameraActor* CreateCamera(FbxCamera* InCamera, UWorld* InWorld);

	// meshes
	/**
	* Fill skeletal mesh data from Fbx Nodes.  If this function needs to triangulate the mesh, then it could invalidate the
	* original FbxMesh pointer.  Hence FbxMesh is a reference so this function can set the new pointer if need be.
	*
	* @param ImportData object to store skeletal mesh data
	* @param FbxMesh	Fbx mesh object belonging to Node
	* @param FbxSkin	Fbx Skin object belonging to FbxMesh
	* @param FbxShape	Fbx Morph object, if not NULL, we are importing a morph object.
	* @param SortedLinks    Fbx Links(bones) of this skeletal mesh
	* @param FbxMatList  All material names of the skeletal mesh
	*
	* @returns bool*	true if import successfully.
	*/
	bool FillSkelMeshImporterFromFbx(FSkeletalMeshImportData& ImportData, FbxMesh*& Mesh, FbxSkin* Skin,
		FbxShape* Shape, TArray<FbxNode*> &SortedLinks, const TArray<FbxSurfaceMaterial*>& FbxMaterials);

	/**
	* Import bones from skeletons that NodeArray bind to.
	*
	* @param NodeArray Fbx Nodes to import, they are bound to the same skeleton system
	* @param ImportData object to store skeletal mesh data
	* @param OutSortedLinks return all skeletons sorted by depth traversal
	* @param bOutDiffPose
	* @param bDisableMissingBindPoseWarning
	* @param bUseTime0AsRefPose	in/out - Use Time 0 as Ref Pose
	*/
	bool ImportBone(TArray<FbxNode*>& NodeArray, FSkeletalMeshImportData &ImportData, UFbxSkeletalMeshImportData* TemplateData, TArray<FbxNode*> &OutSortedLinks, bool& bOutDiffPose, bool bDisableMissingBindPoseWarning, bool & bUseTime0AsRefPose);

	/**
	* Skins the control points of the given mesh or shape using either the default pose for skinning or the first frame of the
	* default animation.  The results are saved as the last X verts in the given FSkeletalMeshBinaryImport
	*
	* @param SkelMeshImporter object to store skeletal mesh data
	* @param FbxMesh	The Fbx mesh object with the control points to skin
	* @param FbxShape	If a shape (aka morph) is provided, its control points will be used instead of the given meshes
	* @param bUseT0	If true, then the pose at time=0 will be used instead of the ref pose
	*/
	void SkinControlPointsToPose(FSkeletalMeshImportData &ImportData, FbxMesh* Mesh, FbxShape* Shape, bool bUseT0);

	// anims
	/**
	* Check if the Fbx node contains animation
	*
	* @param Node Fbx node
	* @return bool true if the Fbx node contains animation.
	*/
	//bool IsAnimated(FbxNode* Node);

	/**
	* Fill each Trace for AnimSequence with Fbx skeleton animation by key
	*
	* @param Node   Fbx skeleton node
	* @param AnimSequence
	* @param TakeName
	* @param bIsRoot if the Fbx skeleton node is root skeleton
	* @param Scale scale factor for this skeleton node
	*/
	bool FillAnimSequenceByKey(FbxNode* Node, UAnimSequence* AnimSequence, const char* TakeName, FbxTime& Start, FbxTime& End, bool bIsRoot, FbxVector4 Scale);
	/*bool CreateMatineeSkeletalAnimation(AExSkelMeshActor* Actor, UAnimSet* AnimSet);
	bool CreateMatineeAnimation(FbxNode* Node, AActor* Actor, bool bInvertOrient, bool bAddDirectorTrack);*/


	// material
	/**
	* Import each material Input from Fbx Material
	*
	* @param FbxMaterial	Fbx material object
	* @param UnrealMaterial
	* @param MaterialProperty The material component to import
	* @param MaterialInput
	* @param bSetupAsNormalMap
	* @param UVSet
	* @return bool
	*/
	bool CreateAndLinkExpressionForMaterialProperty(FbxSurfaceMaterial& FbxMaterial,
		UMaterial* UnrealMaterial,
		const char* MaterialProperty,
		FExpressionInput& MaterialInput,
		bool bSetupAsNormalMap,
		TArray<FString>& UVSet,
		const FVector2f& Location);
	/**
	* Add a basic white diffuse color if no expression is linked to diffuse input.
	*
	* @param unMaterial Unreal material object.
	*/
	void FixupMaterial(FbxSurfaceMaterial& FbxMaterial, UMaterial* unMaterial);

	/**
	* Get material mapping array according "Skinxx" flag in material name
	*
	* @param FSkeletalMeshBinaryImport& The unreal skeletal mesh.
	*/
	void SetMaterialSkinXXOrder(FSkeletalMeshImportData& ImportData);

	/**
	* Create materials from Fbx node.
	* Only setup channels that connect to texture, and setup the UV coordinate of texture.
	* If diffuse channel has no texture, one default node will be created with constant.
	*
	* @param FbxNode  Fbx node
	* @param outMaterials Unreal Materials we created
	* @param UVSets UV set name list
	* @return int32 material count that created from the Fbx node
	*/
	int32 CreateNodeMaterials(FbxNode* FbxNode, TArray<UMaterialInterface*>& outMaterials, TArray<FString>& UVSets);

	/**
	* Make material Unreal asset name from the Fbx material
	*
	* @param FbxMaterial Material from the Fbx node
	* @return Sanitized asset name
	*/
	FString GetMaterialFullName(FbxSurfaceMaterial& FbxMaterial);

	/**
	* Create Unreal material from Fbx material.
	* Only setup channels that connect to texture, and setup the UV coordinate of texture.
	* If diffuse channel has no texture, one default node will be created with constant.
	*
	* @param KFbxSurfaceMaterial*  Fbx material
	* @param outMaterials Unreal Materials we created
	* @param outUVSets
	*/
	void CreateUnrealMaterial(FbxSurfaceMaterial& FbxMaterial, TArray<UMaterialInterface*>& OutMaterials, TArray<FString>& UVSets);

	/**
	* Visit all materials of one node, import textures from materials.
	*
	* @param Node FBX node.
	*/
	void ImportTexturesFromNode(FbxNode* Node);

	/**
	* Generate Unreal texture object from FBX texture.
	*
	* @param FbxTexture FBX texture object to import.
	* @param bSetupAsNormalMap Flag to import this texture as normal map.
	* @return UTexture* Unreal texture object generated.
	*/
	UTexture* ImportTexture(FbxFileTexture* FbxTexture, bool bSetupAsNormalMap);

	/**
	*
	*
	* @param
	* @return UMaterial*
	*/
	//UMaterial* GetImportedMaterial(KFbxSurfaceMaterial* pMaterial);

	/**
	* Check if the meshes in FBX scene contain smoothing group info.
	* It's enough to only check one of mesh in the scene because "Export smoothing group" option affects all meshes when export from DCC.
	* To ensure only check one time, use flag bFirstMesh to record if this is the first mesh to check.
	*
	* @param FbxMesh Fbx mesh to import
	*/
	void CheckSmoothingInfo(FbxMesh* FbxMesh);

	/**
	* check if two faces belongs to same smoothing group
	*
	* @param ImportData
	* @param Face1 one face of the skeletal mesh
	* @param Face2 another face
	* @return bool true if two faces belongs to same group
	*/
	bool FacesAreSmoothlyConnected(FSkeletalMeshImportData &ImportData, int32 Face1, int32 Face2);

	/**
	* Make un-smooth faces work.
	*
	* @param ImportData
	* @return int32 number of points that added when process unsmooth faces
	*/
	int32 DoUnSmoothVerts(FSkeletalMeshImportData &ImportData);

	/**
	* Merge all layers of one AnimStack to one layer.
	*
	* @param AnimStack     AnimStack which layers will be merged
	* @param ResampleRate  resample rate for the animation
	*/
	void MergeAllLayerAnimation(FbxAnimStack* AnimStack, int32 ResampleRate);

	//
	// for matinee export
	//
public:
	/**
	* Retrieves whether there are any unknown camera instances within the FBX document.
	*/
	UNREALED_API bool HasUnknownCameras(AMatineeActor* InMatineeActor) const;

	/**
	* Sets the camera creation flag. Call this function before the import in order to enforce
	* the creation of FBX camera instances that do not exist in the current scene.
	*/
	inline void SetProcessUnknownCameras(bool bCreateMissingCameras)
	{
		bCreateUnknownCameras = bCreateMissingCameras;
	}

	/**
	* Modifies the Matinee actor with the animations found in the FBX document.
	*
	* @return	true, if sucessful
	*/
	UNREALED_API bool ImportMatineeSequence(AMatineeActor* InMatineeActor);


	/** Create a new asset from the package and objectname and class */
	static UObject* CreateAssetOfClass(UClass* AssetClass, FString ParentPackageName, FString ObjectName, bool bAllowReplace = false);

	/* Templated function to create an asset with given package and name */
	template< class T>
	static T * CreateAsset(FString ParentPackageName, FString ObjectName, bool bAllowReplace = false)
	{
		return (T*)CreateAssetOfClass(T::StaticClass(), ParentPackageName, ObjectName, bAllowReplace);
	}

protected:
	bool bCreateUnknownCameras;

	/**
	* Creates a Matinee group for a given actor within a given Matinee actor.
	*/
	UInterpGroupInst* CreateMatineeGroup(AMatineeActor* InMatineeActor, AActor* Actor, FString GroupName);
	/**
	* Imports a FBX scene node into a Matinee actor group.
	*/
	float ImportMatineeActor(FbxNode* FbxNode, UInterpGroupInst* MatineeGroup);

	/**
	* Imports an FBX transform curve into a movement subtrack
	*/
	void ImportMoveSubTrack(FbxAnimCurve* FbxCurve, int32 FbxDimension, UInterpTrackMoveAxis* SubTrack, int32 CurveIndex, bool bNegative, FbxAnimCurve* RealCurve, float DefaultVal);

	/**
	* Imports a FBX animated element into a Matinee track.
	*/
	void ImportMatineeAnimated(FbxAnimCurve* FbxCurve, FInterpCurveVector& Curve, int32 CurveIndex, bool bNegative, FbxAnimCurve* RealCurve, float DefaultVal);
	/**
	* Imports a FBX camera into properties tracks of a Matinee group for a camera actor.
	*/
	void ImportCamera(ACameraActor* Actor, UInterpGroupInst* MatineeGroup, FbxCamera* Camera);
	/**
	* Imports a FBX animated value into a property track of a Matinee group.
	*/
	void ImportAnimatedProperty(float* Value, const TCHAR* ValueName, UInterpGroupInst* MatineeGroup, const float FbxValue, FbxProperty Property, bool bImportFOV = false, FbxCamera* Camera = NULL);
	/**
	* Check if FBX node has transform animation (translation and rotation, not check scale animation)
	*/
	bool IsNodeAnimated(FbxNode* FbxNode, FbxAnimLayer* AnimLayer = NULL);

	/**
	* As movement tracks in Unreal cannot have differing interpolation modes for position & rotation,
	* we consolidate the two modes here.
	*/
	void ConsolidateMovementTrackInterpModes(UInterpTrackMove* MovementTrack);

	/**
	* Get Unreal Interpolation mode from FBX interpolation mode
	*/
	EInterpCurveMode GetUnrealInterpMode(FbxAnimCurveKey FbxKey);

	/**
	* Fill up and verify bone names for animation
	*/
	void FillAndVerifyBoneNames(USkeleton* Skeleton, TArray<FbxNode*>& SortedLinks, TArray<FName> & OutRawBoneNames, FString Filename);
	/**
	* Is valid animation data
	*/
	bool IsValidAnimationData(TArray<FbxNode*>& SortedLinks, TArray<FbxNode*>& NodeArray, int32& ValidTakeCount);

	/**
	* Retrieve pose array from bind pose
	*
	* Iterate through Scene:Poses, and find valid bind pose for NodeArray, and return those Pose if valid
	*
	*/
	bool RetrievePoseFromBindPose(const TArray<FbxNode*>& NodeArray, FbxArray<FbxPose*> & PoseArray) const;

public:
	/** Import and set up animation related data from mesh **/
	void SetupAnimationDataFromMesh(USkeletalMesh * SkeletalMesh, UObject* InParent, TArray<FbxNode*>& NodeArray, UFbxAnimSequenceImportData* ImportData, const FString& Filename);

	/** error message handler */
	void AddTokenizedErrorMessage(TSharedRef<FTokenizedMessage> Error, FName FbxErrorName);
	void ClearTokenizedErrorMessages();
	void FlushToTokenizedErrorMessage(enum EMessageSeverity::Type Severity);

private:
	friend class FFbxLoggerSetter;

	// logger set/clear function
	class FFbxLogger * Logger;
	void SetLogger(class FFbxLogger * InLogger);
	void ClearLogger();

	FImportedMaterialData ImportedMaterialData;

private:
	/**
	* Import FbxCurve to Curve
	*/
	bool ImportCurve(const FbxAnimCurve* FbxCurve, FFloatCurve * Curve, const FbxTimeSpan &AnimTimeSpan, const float ValueScale = 1.f) const;


	/**
	* Import FbxCurve to anim sequence
	*/
	bool ImportCurveToAnimSequence(class UAnimSequence * TargetSequence, const FString& CurveName, const FbxAnimCurve* FbxCurve, int32 CurveFlags, const FbxTimeSpan AnimTimeSpan, const float ValueScale = 1.f) const;
#endif
};
