// Copyright 2015 BlackMa9. All Rights Reserved.

#include "PmdImporter.h"
#include "IM4UPrivatePCH.h"
#include "MMDImportHelper.h"

namespace MMD4UE4
{

	DEFINE_LOG_CATEGORY(LogMMD4UE4_PmdMeshInfo)

		PmdMeshInfo::PmdMeshInfo()
	{
	}


	PmdMeshInfo::~PmdMeshInfo()
	{
	}

	bool PmdMeshInfo::PMDLoaderBinary(
		const uint8 *& Buffer,
		const uint8 * BufferEnd
		)
	{
		////////////////////////////////////////////

		uint32 memcopySize = 0;
		float modelScale = 10.0f;

		memcopySize = sizeof(header);
		FMemory::Memcpy(&header, Buffer, memcopySize);
		// PMDファイルかどうかを確認
		if (header.Magic[0] == 'P' && header.Magic[1] == 'm' && header.Magic[2] == 'd')
		{
			UE_LOG(LogMMD4UE4_PmdMeshInfo, Warning, TEXT("PMX Import START /Correct Magic[PMX]"));
		}
		else
		{
			UE_LOG(LogMMD4UE4_PmdMeshInfo, Error, TEXT("PMX Import FALSE/Return /UnCorrect Magic[PMX]"));
			return false;
		}
		// バージョン１以外は読み込めない
		/*if (*((float *)header.Version) != 0x0000803f)
		{
			//DXST_ERRORLOGFMT_ADD((_T("PMD Load Error : バージョン１．０以外は読み込めません\n")));
			return false;
		}*/
		Buffer += memcopySize;
		// 各データの先頭アドレスをセット
		{
			//頂点
			memcopySize = sizeof(PMD_VERTEX_DATA);
			FMemory::Memcpy(&vertexData, Buffer, memcopySize);
			Buffer += memcopySize;

			memcopySize = 38;// sizeof(PMD_VERTEX);
			vertexList.AddZeroed(vertexData.Count); 
			for (int32 i = 0; i < vertexData.Count; ++i)
			{
				FMemory::Memcpy(&vertexList[i], Buffer, memcopySize);
				Buffer += memcopySize;
			}

		}
		{
			//面データ
			memcopySize = sizeof(PMD_FACE_DATA);
			FMemory::Memcpy(&faceData, Buffer, memcopySize);
			Buffer += memcopySize;

			memcopySize = sizeof(PMD_FACE);
			faceList.AddZeroed(faceData.VertexCount / 3);
			for (uint32 i = 0; i < faceData.VertexCount / 3; ++i)
			{
				FMemory::Memcpy(&faceList[i], Buffer, memcopySize);
				Buffer += memcopySize;
			}

		}
		{
			//マテリアル
			memcopySize = sizeof(PMD_MATERIAL_DATA);
			FMemory::Memcpy(&materialData, Buffer, memcopySize);
			Buffer += memcopySize;

			//memcopySize = 70;//sizeof(PMD_MATERIAL);
			materialList.AddZeroed(materialData.Count);
			for (int32 i = 0; i < materialData.Count; ++i)
			{
				memcopySize = sizeof(float) * 11 + sizeof(uint8) * 2;//DiffuseR --> Edge;
				FMemory::Memcpy(&materialList[i], Buffer, memcopySize);
				Buffer += memcopySize;
				memcopySize = sizeof(int) * 1 + sizeof(char) * 20;//FaceVertexCount --> TextureFileName[20];
				FMemory::Memcpy(&(materialList[i].FaceVertexCount), Buffer, memcopySize);
				Buffer += memcopySize;
			}

		}
		{
			//ボーン
			memcopySize = sizeof(PMD_BONE_DATA);
			FMemory::Memcpy(&boneData, Buffer, memcopySize);
			Buffer += memcopySize;

			//memcopySize = 39;//sizeof(PMD_BONE);
			boneList.AddZeroed(boneData.Count);
			for (int32 i = 0; i < boneData.Count; ++i)
			{
				memcopySize =
					sizeof(char) * 20
					+ sizeof(uint16) * 2
					+ sizeof(uint8) * 1
					+ sizeof(uint16) * 1;//name --> IkParent;
				FMemory::Memcpy(&boneList[i], Buffer, memcopySize);
				Buffer += memcopySize;
				memcopySize =
					sizeof(float) * 3;//name --> IkParent;
				FMemory::Memcpy(&(boneList[i].HeadPos[0]), Buffer, memcopySize);
				Buffer += memcopySize;
			}

		}
		{
			//IK
			memcopySize = sizeof(PMD_IK_DATA);
			FMemory::Memcpy(&ikData, Buffer, memcopySize);
			Buffer += memcopySize;

			//memcopySize = 39;//sizeof(PMD_BONE);
			ikList.AddZeroed(ikData.Count);
			for (int32 i = 0; i < ikData.Count; ++i)
			{
				memcopySize = 0
					+ sizeof(uint16) * 2	//Bone,TargetBone
					+ sizeof(uint8) * 1;		//ChainLength
				FMemory::Memcpy(&ikList[i].Bone, Buffer, memcopySize);
				Buffer += memcopySize;

				memcopySize = 0
					+ sizeof(uint16) * 1	//Iterations
					+ sizeof(float) * 1;	//RotLimit
				FMemory::Memcpy(&ikList[i].Iterations, Buffer, memcopySize);
				Buffer += memcopySize;
				//fix : pmd rotlimit (1.0 == 4.0 rad) -> dig
				ikList[i].RotLimit = FMath::RadiansToDegrees(ikList[i].RotLimit * 4.0);

				uint8 tempChainLength = ikList[i].ChainLength;//Byte ChainLength;
				ikList[i].ChainBoneIndexs.AddZeroed(tempChainLength);
				memcopySize = sizeof(uint16);// *tempChainLength;
				for (int32 k = 0; k < tempChainLength; ++k)
				{
					FMemory::Memcpy(&(ikList[i].ChainBoneIndexs[k]), Buffer, memcopySize);
					Buffer += memcopySize;
				}
			}

		}
		{
			//モーフ
			memcopySize = sizeof(PMD_SKIN_DATA);
			FMemory::Memcpy(&skinData, Buffer, memcopySize);
			Buffer += memcopySize;

			//memcopySize = 39;//sizeof(PMD_BONE);
			skinList.AddZeroed(skinData.Count);
			for (int32 i = 0; i < skinData.Count; ++i)
			{
				memcopySize = 0
					+ sizeof(char) * 20
					+ sizeof(int) * 1
					+ sizeof(uint8) * 1;//Name --> SkinType;
				FMemory::Memcpy(&skinList[i], Buffer, memcopySize);
				Buffer += memcopySize;

				int tempVertexCount = skinList[i].VertexCount;
				skinList[i].Vertex.AddZeroed(tempVertexCount);
				memcopySize = sizeof(PMD_SKIN_VERT);// *tempVertexCount;
				for (int32 k = 0; k < tempVertexCount; ++k)
				{
					FMemory::Memcpy(&(skinList[i].Vertex[k]), Buffer, memcopySize);
					Buffer += memcopySize;
				}
			}

		}
		//表示枠（表情、ボーン）
		//拡張仕様(英名対応)
		//拡張仕様(ToonTexture)
		//拡張仕様(物理：剛体)
		//拡張仕様(物理：Joint)
		//////////////////////////////////////////////
		UE_LOG(LogMMD4UE4_PmdMeshInfo, Warning, TEXT("PMX Importer Class Complete"));
		return true;
	}

	/////////////////////////////////////

	bool PmdMeshInfo::ConvertToPmxFormat(
		PmxMeshInfo * pmxMeshInfoPtr 
		)
	{
		uint32 memcopySize = 0;
		float modelScale = 10.0f;
		PmdMeshInfo * pmdMeshInfoPtr = this;

		pmxMeshInfoPtr->modelNameJP 
			= ConvertMMDSJISToFString((uint8 *)&(header.Name), sizeof(header.Name));

		pmxMeshInfoPtr->modelNameJP 
			= pmxMeshInfoPtr->modelNameJP.Replace(TEXT("."), TEXT("_"));// [.] is broken filepath for ue4 


		pmxMeshInfoPtr->modelCommentJP
			= ConvertMMDSJISToFString((uint8 *)&(header.Comment), sizeof(header.Comment));

		{
			//統計
			uint32 statics_bdef1 = 0;
			uint32 statics_bdef2 = 0;

			pmxMeshInfoPtr->vertexList.AddZeroed(vertexData.Count);
			for (int32 VertexIndex = 0; VertexIndex < vertexData.Count; ++VertexIndex)
			{
				PMD_VERTEX & pmdVertexPtr = vertexList[VertexIndex];
				PMX_VERTEX & pmxVertexPtr = pmxMeshInfoPtr->vertexList[VertexIndex];
				///
				//位置(x,y,z)
				memcopySize = sizeof(pmxVertexPtr.Position);
				FMemory::Memcpy(&pmxVertexPtr.Position, pmdVertexPtr.Position, memcopySize);
				pmxVertexPtr.Position = ConvertVectorAsixToUE4FromMMD(pmxVertexPtr.Position)*modelScale;
				//法線(x,y,z)
				memcopySize = sizeof(pmxVertexPtr.Normal);
				FMemory::Memcpy(&pmxVertexPtr.Normal, pmdVertexPtr.Normal, memcopySize);
				pmxVertexPtr.Normal = ConvertVectorAsixToUE4FromMMD(pmxVertexPtr.Normal);
				//UV(u,v)
				memcopySize = sizeof(pmxVertexPtr.UV);
				FMemory::Memcpy(&pmxVertexPtr.UV, pmdVertexPtr.Uv, memcopySize);
				/*
				float tempUV = pmxVertexPtr.UV.X;//UE4座標系反転
				pmxVertexPtr.UV.X = 1 - pmxVertexPtr.UV.Y;
				pmxVertexPtr.UV.Y = 1 - tempUV;
				*/
				//追加UV(x,y,z,w)  PMXヘッダの追加UV数による	n:追加UV数 0～4

				// ウェイト変形方式 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF
				//pmxVertexPtr.WeightType = 0;
				// ボーンIndexが同じ場合はBDEF1としてあつかう
				/*
				if (pmdVertexPtr.BoneNo[0] != pmdVertexPtr.BoneNo[1]
					|| (pmdVertexPtr.BoneNo[1] == 0 && pmdVertexPtr.BoneWeight == 100))
				{
					//bdef1
					pmxVertexPtr.WeightType = 0;
					pmxVertexPtr.BoneIndex[0] = pmdVertexPtr.BoneNo[0]+1;
					pmxVertexPtr.BoneIndex[1] = pmdVertexPtr.BoneNo[1]+1;
					pmxVertexPtr.BoneWeight[0] = pmdVertexPtr.BoneWeight/100.0f;
					pmxVertexPtr.BoneWeight[1] = (100 - pmdVertexPtr.BoneWeight)/100.0f;
					//
					statics_bdef1++;
				}
				else*/
				{
					//BUG::
					//BDEF1との混在だと何故かウェイト設定が異常であるとMake時に警告が出るため、
					//暫定対処として、PMD形式では一律BDEF2形式とする。データが多くなるよりかは正しく動作することを優先。
					//bdef2
					pmxVertexPtr.WeightType = 1;
					pmxVertexPtr.BoneIndex[0] = pmdVertexPtr.BoneNo[0] + 1;
					pmxVertexPtr.BoneIndex[1] = pmdVertexPtr.BoneNo[1] + 1;
					pmxVertexPtr.BoneWeight[0] = pmdVertexPtr.BoneWeight / 100.0f;
					pmxVertexPtr.BoneWeight[1] = (100 - pmdVertexPtr.BoneWeight) / 100.0f;
					//
					statics_bdef2++;
				}
				//エッジ倍率  材質のエッジサイズに対しての倍率値
			}
			UE_LOG(LogMMD4UE4_PmdMeshInfo, Warning,
				TEXT("PMX convert [Vertex:: statics bone type, bdef1 = %u] Complete"), statics_bdef1);
			UE_LOG(LogMMD4UE4_PmdMeshInfo, Warning,
				TEXT("PMX convert [Vertex:: statics bone type, bdef2 = %u] Complete"), statics_bdef2);
			UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning, TEXT("PMX convert [VertexList] Complete"));
		}
		{
			/*
			●面

			n : 頂点Indexサイズ     | 頂点の参照Index

			※3点(3頂点Index)で1面
			材質毎の面数は材質内の面(頂点)数で管理 (同PMD方式)
			*/
			uint32 PmxFaceNum = 0;
			PmxFaceNum = faceData.VertexCount;
			PmxFaceNum /= 3;

			pmxMeshInfoPtr->faseList.AddZeroed(PmxFaceNum);
			for (uint32 FaceIndex = 0; FaceIndex < PmxFaceNum; ++FaceIndex)
			{
				PMD_FACE & pmdFaceListPtr = faceList[FaceIndex];
				PMX_FACE & pmxFaseListPtr = pmxMeshInfoPtr->faseList[FaceIndex];
				//
				for (int SubNum = 0; SubNum < 3; ++SubNum)
				{
					pmxFaseListPtr.VertexIndex[SubNum]
						= pmdFaceListPtr.VertexIndx[SubNum];
				}
			}
			UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning, TEXT("PMX convert [faceList] Complete"));
		}
		/*
		{

			// テクスチャの数を取得
			uint32 PmxTextureNum = 0;
			//
			memcopySize = sizeof(PmxTextureNum);
			FMemory::Memcpy(&PmxTextureNum, Buffer, memcopySize);
			Buffer += memcopySize;

			// テクスチャデータを格納するメモリ領域の確保
			textureList.AddZeroed(PmxTextureNum);

			// テクスチャの情報を取得
			for (uint32 i = 0; i < PmxTextureNum; i++)
			{
				textureList[i].TexturePath = PMXTexBufferToFString(&Buffer, pmxEncodeType);
			}
			UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning, TEXT("PMX Import [textureList] Complete"));
		}
		*/
		{
			FString tempAlphaStr;
			TArray<FString> tempTexPathList;
			// マテリアルの数を取得
			uint32 PmxMaterialNum = 0;
			//
			PmxMaterialNum = materialData.Count;

			// マテリアルデータを格納するメモリ領域の確保
			pmxMeshInfoPtr->materialList.AddZeroed(PmxMaterialNum);

			// マテリアルの読みこみ
			for (uint32 i = 0; i < PmxMaterialNum; i++)
			{
				PMD_MATERIAL & pmdMaterialPtr = materialList[i];
				PMX_MATERIAL & pmxMaterialPtr = pmxMeshInfoPtr->materialList[i];

				// 材質名の取得
				pmxMaterialPtr.Name = FString::Printf(TEXT("mat_%d"), i);
				pmxMaterialPtr.NameEng = pmxMaterialPtr.Name;

				//Diffuse (R,G,B,A)
				pmxMaterialPtr.Diffuse[0] = pmdMaterialPtr.DiffuseR;
				pmxMaterialPtr.Diffuse[1] = pmdMaterialPtr.DiffuseG;
				pmxMaterialPtr.Diffuse[2] = pmdMaterialPtr.DiffuseB;
				pmxMaterialPtr.Diffuse[3] = pmdMaterialPtr.Alpha;
				//Specular (R,G,B))
				pmxMaterialPtr.Specular[0] = pmdMaterialPtr.SpecularR;
				pmxMaterialPtr.Specular[1] = pmdMaterialPtr.SpecularG;
				pmxMaterialPtr.Specular[2] = pmdMaterialPtr.SpecularB;
				//Specular係数
				pmxMaterialPtr.SpecularPower = pmdMaterialPtr.Specularity;
				//Ambient (R,G,B)
				pmxMaterialPtr.Ambient[0] = pmdMaterialPtr.AmbientR;
				pmxMaterialPtr.Ambient[1] = pmdMaterialPtr.AmbientG;
				pmxMaterialPtr.Ambient[2] = pmdMaterialPtr.AmbientB;


				/*
				描画フラグ(8bit) - 各bit 0:OFF 1:ON
				0x01:両面描画, 0x02:地面影, 0x04:セルフシャドウマップへの描画, 0x08:セルフシャドウの描画,
				0x10:エッジ描画
				*/
				tempAlphaStr = FString::Printf(TEXT("%.03f"), pmdMaterialPtr.Alpha);
				//pmxMaterialPtr.CullingOff = (pmdMaterialPtr.Alpha < 1.0f) ? 1 : 0;//本来の仕様のはず？だが裏地に黒エッジ出来ないので1.0fだと透ける
				pmxMaterialPtr.CullingOff = 1;//上記理由からPMDの場合両面にする。あとで適宜片面にするなどドローコールを減らしてもらいたい。。。
				pmxMaterialPtr.GroundShadow = (0) ? 1 : 0;
				pmxMaterialPtr.SelfShadowMap = tempAlphaStr.Equals("0.980") ? 1 : 0;
				pmxMaterialPtr.SelfShadowDraw = tempAlphaStr.Equals( "0.980") ? 1 : 0;
				pmxMaterialPtr.EdgeDraw = (0) ? 1 : 0;

				//エッジ色 (R,G,B,A)
				pmxMaterialPtr.EdgeColor[0] = 0;
				pmxMaterialPtr.EdgeColor[1] = 0;
				pmxMaterialPtr.EdgeColor[2] = 0;
				pmxMaterialPtr.EdgeColor[3] = 0;

				//エッジサイズ
				pmxMaterialPtr.EdgeSize = 0;


				//スフィアモード 0:無効 1:乗算(sph) 2:加算(spa) 
				//3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)
				pmxMaterialPtr.SphereMode = 0;//初期値
				
				PMX_TEXTURE tempTex;
				FString tempTexPathStr;
				FString tempShaPathStr;
				tempTex.TexturePath
					= ConvertMMDSJISToFString(
						(uint8 *)pmdMaterialPtr.TextureFileName,
						sizeof(pmdMaterialPtr.TextureFileName));
				if (tempTex.TexturePath.Split("/", &tempTexPathStr, &tempShaPathStr))
				{
					//サブフォルダにマテリアルがない前提
					tempTex.TexturePath = tempTexPathStr;
					if (tempTex.TexturePath.Find(".spa") != -1)
					{
						//加算でない場合
						pmxMaterialPtr.SphereMode = 1;
						UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning,
							TEXT("PMX convert [materialList] multi texture[%s] / sphere[%s]")
							, *tempTex.TexturePath
							, *tempShaPathStr);
					}
					else
					{
						pmxMaterialPtr.SphereMode = 2;
						UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning,
							TEXT("PMX convert [materialList] add texture[%s] / sphere[%s]")
							, *tempTex.TexturePath
							, *tempShaPathStr);
					}
				}
				else if (tempTex.TexturePath.Split("*", &tempTexPathStr, &tempShaPathStr))
				{
					tempTex.TexturePath = tempTexPathStr;
					if (tempTex.TexturePath.Find(".spa") != -1)
					{
						//加算でない場合
						pmxMaterialPtr.SphereMode = 1;
						UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning,
							TEXT("PMX convert [materialList] multi texture[%s] / sphere[%s]")
							, *tempTex.TexturePath
							, *tempShaPathStr);
					}
					else
					{
						pmxMaterialPtr.SphereMode = 2;
						UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning,
							TEXT("PMX convert [materialList] add texture[%s] / sphere[%s]")
							, *tempTex.TexturePath
							, *tempShaPathStr);
					}
				}
				else
				{
					//テクスチャが一つのみ
					if (tempTex.TexturePath.Find(".sp") != -1)
					{
						//スフィアのみ
						tempShaPathStr = tempTex.TexturePath;
						tempTex.TexturePath = "";
						if (tempTex.TexturePath.Find(".spa") != -1)
						{
							//加算でない場合
							pmxMaterialPtr.SphereMode = 1;
						}
						else
						{
							pmxMaterialPtr.SphereMode = 2;
						}
					}
					else
					{
						//スフィアなし
						tempShaPathStr = "";
					}
					UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning,
						TEXT("PMX convert [materialList] texture[%s] mono sphere[%s]")
						, *tempTex.TexturePath
						, *tempShaPathStr);
					//tempTex.TexturePath = tempTexPathStr;
				}
				//通常テクスチャ, テクスチャテーブルの参照Index
				if (tempTex.TexturePath.Equals(""))
				{
					//not
					pmxMaterialPtr.TextureIndex = -1;
				}
				else
				{
					pmxMaterialPtr.TextureIndex
						//= tempTexPathList.AddUnique(pmdMaterialPtr.TextureFileName);
						= tempTexPathList.AddUnique(tempTex.TexturePath);
					//pmxMeshInfoPtr->textureList.Add/*Unique*/(tempTex);
					if (pmxMaterialPtr.TextureIndex > pmxMeshInfoPtr->textureList.Num() - 1)
					{
						pmxMeshInfoPtr->textureList.Add(tempTex);
					}
				}
				if (tempShaPathStr.Equals(""))
				{
					//not
					//スフィアテクスチャ, テクスチャテーブルの参照Index  ※テクスチャ拡張子の制限なし
					pmxMaterialPtr.SphereTextureIndex
						= -1;
				}
				else
				{

					PMX_TEXTURE tempSphTex;
					tempSphTex.TexturePath = tempShaPathStr;
					pmxMaterialPtr.SphereTextureIndex
						//= tempTexPathList.AddUnique(pmdMaterialPtr.TextureFileName);
						= tempTexPathList.AddUnique(tempSphTex.TexturePath);
					//pmxMeshInfoPtr->textureList.Add/*Unique*/(tempTex);
					if (pmxMaterialPtr.SphereTextureIndex > pmxMeshInfoPtr->textureList.Num() - 1)
					{
						pmxMeshInfoPtr->textureList.Add(tempSphTex);
					}
				}
				//共有Toonフラグ 0:継続値は個別Toon 1 : 継続値は共有Toon
				pmxMaterialPtr.ToonFlag = 1;

				if (pmxMaterialPtr.ToonFlag == 0)
				{//Toonテクスチャ, テクスチャテーブルの参照Index
					pmxMaterialPtr.ToonTextureIndex
						= 0;
				}
				else
				{//共有Toonテクスチャ[0～9] -> それぞれ toon01.bmp～toon10.bmp に対応
					pmxMaterialPtr.ToonTextureIndex = pmdMaterialPtr.ToolImage;
				}

				// メモはスキップ

				//材質に対応する面(頂点)数 (必ず3の倍数になる)
				pmxMaterialPtr.MaterialFaceVerticeNum = pmdMaterialPtr.FaceVertexCount;
			}
			UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning, TEXT("PMX convert [materialList] Complete"));
		}
		{
			// すべての親BoneをUE4向けに追加
			// ボーンデータを格納するメモリ領域の確保
			pmxMeshInfoPtr->boneList.AddZeroed(1);

			// ボーンの数を取得
			uint32 PmxBoneNum = 0;
			uint32 offsetBoneIndx = 1;

			// ボーン情報の取得
			{
				pmxMeshInfoPtr->boneList[PmxBoneNum].Name = TEXT("Root");
				pmxMeshInfoPtr->boneList[PmxBoneNum].NameEng = "AllTopRootBone";
				pmxMeshInfoPtr->boneList[PmxBoneNum].Position = FVector3f(0);
				pmxMeshInfoPtr->boneList[PmxBoneNum].ParentBoneIndex = INDEX_NONE;
			}
			PmxBoneNum = boneData.Count;
			// ボーンデータを格納するメモリ領域の確保
			pmxMeshInfoPtr->boneList.AddZeroed(PmxBoneNum);

			// ボーン情報の取得
			uint32 PmxIKNum = 0;
			for (uint32 i = offsetBoneIndx; i < PmxBoneNum + offsetBoneIndx; i++)
			{
				PMD_BONE & pmdBonePtr = boneList[i - 1];
				PMX_BONE & pmxBonePtr = pmxMeshInfoPtr->boneList[i];

				pmxBonePtr.Name
					= ConvertMMDSJISToFString((uint8 *)&(pmdBonePtr.Name), sizeof(pmdBonePtr.Name));
				pmxBonePtr.NameEng
					= pmxBonePtr.Name;
				//
				memcopySize = sizeof(pmxBonePtr.Position);
				FMemory::Memcpy(&pmxBonePtr.Position, pmdBonePtr.HeadPos, memcopySize);
				pmxBonePtr.Position = ConvertVectorAsixToUE4FromMMD(pmxBonePtr.Position) * modelScale;

				pmxBonePtr.ParentBoneIndex
					= pmdBonePtr.Parent + offsetBoneIndx;

				//
				if (pmdBonePtr.TailPosBone != -1)
				{
					pmxBonePtr.LinkBoneIndex
						= pmdBonePtr.TailPosBone + offsetBoneIndx;
				}
				else
				{
					pmxBonePtr.LinkBoneIndex = -1;
				}

				//bone type
				switch (pmdBonePtr.Type)
				{
				case 0://0 : 回転
					pmxBonePtr.Flag_EnableRot = 1;
					break;
				case 1: //1:回転と移動
					pmxBonePtr.Flag_EnableRot = 1;
					pmxBonePtr.Flag_EnableMov = 1;
					break;
				case 2: //2:IK
					pmxBonePtr.Flag_IK = 1;
					break;
				case 3: //3:不明？
					break;
				case 4: //4:IK影響下
					break;
				case 5: //5:回転影響下
					break;
				case 6: //6:IK接続先
					break;
				case 7: //7:非表示
					break;
				case 8: //8:捻り
					break;
				case 9: //9:回転運動 
					break;
				default:
					break;
				}
#if 0
				//
				memcopySize = sizeof(pmxBonePtr.TransformLayer);
				FMemory::Memcpy(&pmxBonePtr.TransformLayer, Buffer, memcopySize);
				Buffer += memcopySize;

				uint16 Flag;
				//
				memcopySize = sizeof(Flag);
				FMemory::Memcpy(&Flag, Buffer, memcopySize);
				Buffer += memcopySize;

				pmxBonePtr.Flag_LinkDest = (Flag & 0x0001) != 0 ? 1 : 0;
				pmxBonePtr.Flag_EnableRot = (Flag & 0x0002) != 0 ? 1 : 0;
				pmxBonePtr.Flag_EnableMov = (Flag & 0x0004) != 0 ? 1 : 0;
				pmxBonePtr.Flag_Disp = (Flag & 0x0008) != 0 ? 1 : 0;
				pmxBonePtr.Flag_EnableControl = (Flag & 0x0010) != 0 ? 1 : 0;
				pmxBonePtr.Flag_IK = (Flag & 0x0020) != 0 ? 1 : 0;
				pmxBonePtr.Flag_AddRot = (Flag & 0x0100) != 0 ? 1 : 0;
				pmxBonePtr.Flag_AddMov = (Flag & 0x0200) != 0 ? 1 : 0;
				pmxBonePtr.Flag_LockAxis = (Flag & 0x0400) != 0 ? 1 : 0;
				pmxBonePtr.Flag_LocalAxis = (Flag & 0x0800) != 0 ? 1 : 0;
				pmxBonePtr.Flag_AfterPhysicsTransform = (Flag & 0x1000) != 0 ? 1 : 0;
				pmxBonePtr.Flag_OutParentTransform = (Flag & 0x2000) != 0 ? 1 : 0;

				if (pmxBonePtr.Flag_LinkDest == 0)
				{
					//
					memcopySize = sizeof(pmxBonePtr.OffsetPosition);
					FMemory::Memcpy(&pmxBonePtr.OffsetPosition, Buffer, memcopySize);
					pmxBonePtr.OffsetPosition = ConvertVectorAsixToUE4FromMMD(pmxBonePtr.OffsetPosition) *modelScale;
					Buffer += memcopySize;
				}
				else
				{
					pmxBonePtr.LinkBoneIndex
						= PMXExtendBufferSizeToUint32(&Buffer, this->baseHeader.BoneIndexSize);
				}

				if (pmxBonePtr.Flag_AddRot == 1 || pmxBonePtr.Flag_AddMov == 1)
				{
					pmxBonePtr.AddParentBoneIndex
						= PMXExtendBufferSizeToUint32(&Buffer, this->baseHeader.BoneIndexSize);
					//
					memcopySize = sizeof(pmxBonePtr.AddRatio);
					FMemory::Memcpy(&pmxBonePtr.AddRatio, Buffer, memcopySize);
					Buffer += memcopySize;
				}

				if (pmxBonePtr.Flag_LockAxis == 1)
				{
					//
					memcopySize = sizeof(pmxBonePtr.LockAxisVector);
					FMemory::Memcpy(&pmxBonePtr.LockAxisVector, Buffer, memcopySize);
					Buffer += memcopySize;
				}

				if (pmxBonePtr.Flag_LocalAxis == 1)
				{

					//
					memcopySize = sizeof(pmxBonePtr.LocalAxisXVector);
					FMemory::Memcpy(&pmxBonePtr.LocalAxisXVector, Buffer, memcopySize);
					Buffer += memcopySize;

					//
					memcopySize = sizeof(pmxBonePtr.LocalAxisZVector);
					FMemory::Memcpy(&pmxBonePtr.LocalAxisZVector, Buffer, memcopySize);
					Buffer += memcopySize;
				}

				if (pmxBonePtr.Flag_OutParentTransform == 1)
				{
					//
					memcopySize = sizeof(pmxBonePtr.OutParentTransformKey);
					FMemory::Memcpy(&pmxBonePtr.OutParentTransformKey, Buffer, memcopySize);
					Buffer += memcopySize;
				}
#endif
#if 1
				if (pmxBonePtr.Flag_IK == 1)
				{

					//search ik-list from pmd data.
					PMD_IK * tempPmdIKPtr = NULL;
					for (int32 listIKIndx = 0; listIKIndx < pmdMeshInfoPtr->ikData.Count; ++listIKIndx)
					{
						if (pmdMeshInfoPtr->ikList[listIKIndx].Bone == (i - 1))
						{
							tempPmdIKPtr = &(pmdMeshInfoPtr->ikList[listIKIndx]);
						}
					}
					if (tempPmdIKPtr)
					{
						PmxIKNum++;

						pmxBonePtr.IKInfo.TargetBoneIndex
							= tempPmdIKPtr->TargetBone + offsetBoneIndx;
						//
						pmxBonePtr.IKInfo.LoopNum = tempPmdIKPtr->Iterations; //const 40?

						//
						pmxBonePtr.IKInfo.RotLimit = tempPmdIKPtr->RotLimit;

						//
						pmxBonePtr.IKInfo.LinkNum = tempPmdIKPtr->ChainLength;
						if (pmxBonePtr.IKInfo.LinkNum >= PMX_MAX_IKLINKNUM)
						{
							return false;
						}

						for (int32 j = 0; j < pmxBonePtr.IKInfo.LinkNum; j++)
						{
							pmxBonePtr.IKInfo.Link[j].BoneIndex
								= tempPmdIKPtr->ChainBoneIndexs[j] + offsetBoneIndx;

							FString tempChldBoneName
								= ConvertMMDSJISToFString((uint8 *)&(boneList[tempPmdIKPtr->ChainBoneIndexs[j]].Name),
								sizeof(boneList[tempPmdIKPtr->ChainBoneIndexs[j]].Name));
							const char hiza[20] = "ひざ";
							FString tempHizaName
								= ConvertMMDSJISToFString((uint8 *)&(hiza),
								sizeof(hiza));
							//膝の場合、X軸(MMD)で軸制限を書ける
							if (tempChldBoneName.Find(tempHizaName) != -1)
							{
								pmxBonePtr.IKInfo.Link[j].RotLockFlag = 1;

								pmxBonePtr.IKInfo.Link[j].RotLockMin[0] = -180;//x
								pmxBonePtr.IKInfo.Link[j].RotLockMin[1] = 0;//y
								pmxBonePtr.IKInfo.Link[j].RotLockMin[2] = 0;//z
								//
								pmxBonePtr.IKInfo.Link[j].RotLockMax[0] = -0.5;//x
								pmxBonePtr.IKInfo.Link[j].RotLockMax[1] = 0;//y
								pmxBonePtr.IKInfo.Link[j].RotLockMax[2] = 0;//z
							}
							else
							{

								pmxBonePtr.IKInfo.Link[j].RotLockFlag = 0;
							}
						}
					}
				}
#endif
			}
			UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning, TEXT("PMX convert [BoneList] Complete"));
		}
		{
			//IK
		}
		{
			int32 i, j;
			// モーフ情報の数を取得
			int32 PmxMorphNum = 0;
			TArray<int32> pmdMorphIndexList;
			PMD_SKIN * basePmdMorphPtr = NULL; //base
			PMD_SKIN * targetPmdMorphPtr = NULL; //モーフ変換時の変換元モーフ情報

			for (i = 0; i < pmdMeshInfoPtr->skinData.Count; i++)
			{
				if (0 < pmdMeshInfoPtr->skinList[i].SkinType
					&&  pmdMeshInfoPtr->skinList[i].SkinType < 4)
				{
					//type 0: base除外。それ以外の有効Skinなら加算
					//1: まゆ
					//2: 目
					//3: リップ
					//4: その他
					PmxMorphNum++;
					pmdMorphIndexList.Add(i);
				}
				else if (0 == pmdMeshInfoPtr->skinList[i].SkinType && basePmdMorphPtr == NULL)
				{
					//もし既にBaseが登録されている場合(PMDフォーマットとして異常)は上書きしない
					basePmdMorphPtr = &pmdMeshInfoPtr->skinList[i];
				}
				else
				{
					//Err
				}
			}
			// モーフありかつBaseモーフありの場合、モーフ登録をする
			if (PmxMorphNum > 0 && basePmdMorphPtr )
			{
				// モーフデータを格納するメモリ領域の確保
				pmxMeshInfoPtr->morphList.AddZeroed(PmxMorphNum);

				// モーフ情報の読み込み
				int32 PmxSkinNum = 0;
				FVector3f tempVec;
				for (i = 0; i < PmxMorphNum; i++)
				{
					// target morph ptr 参照
					targetPmdMorphPtr = &pmdMeshInfoPtr->skinList[pmdMorphIndexList[i]];
					//
					pmxMeshInfoPtr->morphList[i].Name
						= ConvertMMDSJISToFString(
							(uint8 *)&(targetPmdMorphPtr->Name),
							sizeof(targetPmdMorphPtr->Name)
							);
					pmxMeshInfoPtr->morphList[i].NameEng
						= pmxMeshInfoPtr->morphList[i].Name;

					//
					pmxMeshInfoPtr->morphList[i].ControlPanel = targetPmdMorphPtr->SkinType;
					//
					pmxMeshInfoPtr->morphList[i].Type = 1;//頂点固定
					//
					pmxMeshInfoPtr->morphList[i].DataNum = targetPmdMorphPtr->VertexCount;

					switch (pmxMeshInfoPtr->morphList[i].Type)
					{
					case 1:	// 頂点
						PmxSkinNum++;
						pmxMeshInfoPtr->morphList[i].Vertex.AddZeroed(pmxMeshInfoPtr->morphList[i].DataNum);

						for (j = 0; j < pmxMeshInfoPtr->morphList[i].DataNum; j++)
						{
							pmxMeshInfoPtr->morphList[i].Vertex[j].Index =
								basePmdMorphPtr->Vertex[targetPmdMorphPtr->Vertex[j].TargetVertexIndex].TargetVertexIndex;
							//
							tempVec.X = targetPmdMorphPtr->Vertex[j].Position[0];
							tempVec.Y = targetPmdMorphPtr->Vertex[j].Position[1];
							tempVec.Z = targetPmdMorphPtr->Vertex[j].Position[2];
							tempVec = ConvertVectorAsixToUE4FromMMD(tempVec)*modelScale;

							pmxMeshInfoPtr->morphList[i].Vertex[j].Offset[0] = tempVec.X;
							pmxMeshInfoPtr->morphList[i].Vertex[j].Offset[1] = tempVec.Y;
							pmxMeshInfoPtr->morphList[i].Vertex[j].Offset[2] = tempVec.Z;
						}
						break;
					default:
						//un support ppmd
						break;
					}
				}
			}
			UE_LOG(LogMMD4UE4_PmxMeshInfo, Warning, TEXT("PMX convert [MorphList] Complete"));
		}
		return true;
	}


}