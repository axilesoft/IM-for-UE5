// Copyright 2015 BlackMa9. All Rights Reserved.

#include "VmdImporter.h"
#include "IM4UPrivatePCH.h"
#include "MMDImportHelper.h"

#include "Animation/AnimSequenceBase.h"

namespace MMD4UE4
{

	DEFINE_LOG_CATEGORY(LogMMD4UE4_VmdMotionInfo)

	VmdMotionInfo::VmdMotionInfo()
	{
		maxFrame = 0;
		minFrame = 0;
	}


	VmdMotionInfo::~VmdMotionInfo()
	{
	}


	bool VmdMotionInfo::VMDLoaderBinary(
		const uint8 *& Buffer,
		const uint8 * BufferEnd
		)
	{
		VmdReadMotionData readData;
		{
			uint32 memcopySize = 0;
			float modelScale = 10.0f;

			memcopySize = sizeof(readData.vmdHeader);
			FMemory::Memcpy(&readData.vmdHeader, Buffer, memcopySize);
			// VMDファイルかどうかを確認 暫定版
			if (readData.vmdHeader.header[0] == 'V' &&
				readData.vmdHeader.header[1] == 'o' && 
				readData.vmdHeader.header[2] == 'c')
			{
				UE_LOG(LogMMD4UE4_VmdMotionInfo, Warning, TEXT("VMD Import START /Correct Magic[Vocaloid Motion Data 0002]"));
			}
			else
			{
				//UE_LOG(LogMMD4UE4_PmdMeshInfo, Error, TEXT("PMX Import FALSE/Return /UnCorrect Magic[PMX]"));
				return false;
			}
			Buffer += memcopySize;

			// 各データの先頭アドレスをセット
			{
				//Key VMD
				memcopySize = sizeof(readData.vmdKeyCount);
				FMemory::Memcpy(&readData.vmdKeyCount, Buffer, memcopySize);
				Buffer += memcopySize;

				//memcopySize = sizeof(VMD_KEY);//111
				readData.vmdKeyList.AddZeroed(readData.vmdKeyCount);
				for (int32 i = 0; i < readData.vmdKeyCount; ++i)
				{
					VMD_KEY * vmdKeyPtr = &readData.vmdKeyList[i];

					//Bone NAME
					memcopySize = sizeof(vmdKeyPtr->Name);
					FMemory::Memcpy(&vmdKeyPtr->Name[0], Buffer, memcopySize);
					Buffer += memcopySize;
					//posandQurt
					memcopySize = sizeof(int32) + sizeof(float)* (4 + 3);
					FMemory::Memcpy(&vmdKeyPtr->Frame, Buffer, memcopySize);
					Buffer += memcopySize;
					//bezier
					memcopySize = sizeof(vmdKeyPtr->Bezier) ;
					FMemory::Memcpy(&vmdKeyPtr->Bezier, Buffer, memcopySize);
					Buffer += memcopySize;
					//
					//dummy bezier
					Buffer += 48 * sizeof(uint8);
				}
			}
			// 各データの先頭アドレスをセット
			{
				//Key Fase VMD
				memcopySize = sizeof(readData.vmdFaceCount);
				FMemory::Memcpy(&readData.vmdFaceCount, Buffer, memcopySize);
				Buffer += memcopySize;

				//memcopySize = sizeof(VMD_FACE_KEY);//111
				readData.vmdFaceList.AddZeroed(readData.vmdFaceCount);
				for (int32 i = 0; i < readData.vmdFaceCount; ++i)
				{
					VMD_FACE_KEY * vmdFacePtr = &readData.vmdFaceList[i];

					//Bone NAME
					memcopySize = sizeof(vmdFacePtr->Name);
					FMemory::Memcpy(&vmdFacePtr->Name[0], Buffer, memcopySize);
					Buffer += memcopySize;
					//frame and value
					memcopySize = sizeof(int32) + sizeof(float);
					FMemory::Memcpy(&vmdFacePtr->Frame, Buffer, memcopySize);
					Buffer += memcopySize;
				}
			}
			// 各データの先頭アドレスをセット
			{
				//Key Camera VMD
				memcopySize = sizeof(readData.vmdCameraCount);
				FMemory::Memcpy(&readData.vmdCameraCount, Buffer, memcopySize);
				Buffer += memcopySize;

				if (readData.vmdCameraCount > 0)
				{
					//memcopySize = sizeof(VMD_CAMERA);//61
					readData.vmdCameraList.AddZeroed(readData.vmdCameraCount);
					for (int32 i = 0; i < readData.vmdCameraCount; ++i)
					{
						VMD_CAMERA * vmdCameraPtr = &readData.vmdCameraList[i];

						//Freme No
						memcopySize = sizeof(uint32)*(1) //FrameNo
							+ sizeof(float)*(1 + 3 + 3);	//Length + Location + Rotate
						FMemory::Memcpy(&vmdCameraPtr->Frame, Buffer, memcopySize);
						Buffer += memcopySize;

						//Interpolation[6][4]
						memcopySize = sizeof(uint8) * (6 * 4);
						FMemory::Memcpy(&vmdCameraPtr->Interpolation[0][0][0], Buffer, memcopySize);
						Buffer += memcopySize;

						//ViewingAngle + Perspective
						memcopySize = sizeof(uint32) * (1)	//ViewingAngle
							+ sizeof(uint8)* (1);			//Perspective
						FMemory::Memcpy(&vmdCameraPtr->ViewingAngle, Buffer, memcopySize);
						Buffer += memcopySize;
					}
				}
			}
		}
		//////////////////////////////
		if (!ConvertVMDFromReadData(&readData))
		{
			// convert err
			return false;
		}
		
		return true;
	}

	bool VmdMotionInfo::ConvertVMDFromReadData(
		VmdReadMotionData * readData
		)
	{
		check(readData);
		if (!readData)
		{
			return false;
		}
		////////////////////////////
		ModelName = ConvertMMDSJISToFString(
			(uint8 *)&(readData->vmdHeader.modelName),
			sizeof(readData->vmdHeader.modelName)
			);
		///////////////////////////////

		int						arrayIndx = -1;
		int						arrayIndxPre;
		FString					trackName;

		TArray<FString>			tempTrackNameList;
		//////////////////////////////////
		{
			//Keys
			TArray<VmdKeyTrackList>	tempKeyBoneList;
			VmdKeyTrackList * vmdKeyTrackPtr = NULL;
			//VMD_KEY * vmdKeyPtr = NULL;

			//VMD Key
			tempTrackNameList.Empty();
			arrayIndx = -1;
			for (int32 i = 0; i < readData->vmdKeyCount; i++)
			{
				// get ptr
				VMD_KEY * vmdKeyPtr = &(readData->vmdKeyList[i]);
				//
				trackName = ConvertMMDSJISToFString(
					(uint8 *)&(vmdKeyPtr->Name),
					sizeof(vmdKeyPtr->Name)
					);
				arrayIndxPre = tempTrackNameList.Num();
				arrayIndx = tempTrackNameList.AddUnique(trackName);
				if (tempTrackNameList.Num() > arrayIndxPre)
				{
					//New Track
					arrayIndx = tempKeyBoneList.Add(VmdKeyTrackList());
					vmdKeyTrackPtr = &(tempKeyBoneList[arrayIndx]);
					vmdKeyTrackPtr->TrackName = trackName;
				}
				else
				{
					vmdKeyTrackPtr = &(tempKeyBoneList[arrayIndx]);
					/*
					//exist
					for (int k = 0; k < tempKeyBoneList.Num(); k++)
					{
					vmdKeyTrackPtr = &(tempKeyBoneList[k]);
					if (vmdKeyTrackPtr->TrackName.Equals(trackName))
					{
					break;
					}
					vmdKeyTrackPtr = NULL;
					}*/
				}
				check(vmdKeyTrackPtr);
				///
				arrayIndx = vmdKeyTrackPtr->keyList.Add(*vmdKeyPtr);
				//
				vmdKeyTrackPtr->maxFrameCount
					= FMath::Max(vmdKeyPtr->Frame, vmdKeyTrackPtr->maxFrameCount);
				vmdKeyTrackPtr->minFrameCount
					= FMath::Min(vmdKeyPtr->Frame, vmdKeyTrackPtr->minFrameCount);
				///
				maxFrame = FMath::Max(vmdKeyPtr->Frame, maxFrame);
				minFrame = FMath::Min(vmdKeyPtr->Frame, minFrame);
			}
			// Frame順に計算しやすいようにListのIndex参照をソートした配列を生成する。ただし無駄すぎる処理。VMDが順不同の為。
			for (int i = 0; i < tempKeyBoneList.Num(); i++)
			{// all bone
				if (tempKeyBoneList[i].keyList.Num() > 0)
				{
					//init : insert array index = 0
					tempKeyBoneList[i].sortIndexList.Add(0);
				}
				else
				{
					continue;
				}
				//sort all vmd-key frames
				for (int k = 1; k < tempKeyBoneList[i].keyList.Num(); k++)
				{
					bool isSetList = false;
					//TBD:2分探索で軽量に行くべきだが面倒くさいのでとりあえず、線形探索にする
					for (int s = 0; s < tempKeyBoneList[i].sortIndexList.Num(); s++)
					{
						//もし現在の位置よりも今回の値が大きいか？
						if (tempKeyBoneList[i].keyList[k].Frame <
							tempKeyBoneList[i].keyList[tempKeyBoneList[i].sortIndexList[s]].Frame)
						{
							/*if (s + 1 == tempKeyBoneList[i].sortIndexList.Num())
							{
							//insert array index = k;
							tempKeyBoneList[i].sortIndexList.Add(k);
							}
							else*/
							{
								//insert array index = k;
								tempKeyBoneList[i].sortIndexList.Insert(k, s);
							}
							//check list size == now index num
							//check(tempKeyBoneList[i].sortIndexList.Num() == k+1);
							isSetList = true;
							break;
						}
					}
					if (!isSetList)
					{
						//add last
						tempKeyBoneList[i].sortIndexList.Add(k);
					}
				}
			}
			keyBoneList = tempKeyBoneList;
		}
		{
			//Skins
			TArray<VmdFaceTrackList> tempKeyFaceList;
			VmdFaceTrackList * vmdFaceTrackPtr = NULL;
			//VMD_FACE_KEY * vmdFacePtr = NULL;

			//Facc
			tempTrackNameList.Empty();
			arrayIndx = -1;
			for (int32 i = 0; i < readData->vmdFaceCount; i++)
			{
				// get ptr
				VMD_FACE_KEY * vmdFacePtr = &(readData->vmdFaceList[i]);
				//
				trackName = ConvertMMDSJISToFString(
					(uint8 *)&(vmdFacePtr->Name),
					sizeof(vmdFacePtr->Name)
					);
				arrayIndxPre = tempTrackNameList.Num();
				arrayIndx = tempTrackNameList.AddUnique(trackName);
				if (tempTrackNameList.Num() > arrayIndxPre)
				{
					//New Track
					arrayIndx = tempKeyFaceList.Add(VmdFaceTrackList());
					vmdFaceTrackPtr = &(tempKeyFaceList[arrayIndx]);
					vmdFaceTrackPtr->TrackName = trackName;
				}
				else
				{
					vmdFaceTrackPtr = &(tempKeyFaceList[arrayIndx]);
					/*
					//exist
					for (int k = 0; k < tempKeyBoneList.Num(); k++)
					{
					vmdKeyTrackPtr = &(tempKeyBoneList[k]);
					if (vmdKeyTrackPtr->TrackName.Equals(trackName))
					{
					break;
					}
					vmdKeyTrackPtr = NULL;
					}*/
				}
				check(vmdFaceTrackPtr);
				///
				arrayIndx = vmdFaceTrackPtr->keyList.Add(*vmdFacePtr);
				//
				vmdFaceTrackPtr->maxFrameCount
					= FMath::Max(vmdFacePtr->Frame, vmdFaceTrackPtr->maxFrameCount);
				vmdFaceTrackPtr->minFrameCount
					= FMath::Min(vmdFacePtr->Frame, vmdFaceTrackPtr->minFrameCount);
				///
				maxFrame = FMath::Max(vmdFacePtr->Frame, maxFrame);
				minFrame = FMath::Min(vmdFacePtr->Frame, minFrame);
			}
			// Frame順に計算しやすいようにListのIndex参照をソートした配列を生成する。ただし無駄すぎる処理。VMDが順不同の為。
			for (int i = 0; i < tempKeyFaceList.Num(); i++)
			{// all bone
				if (tempKeyFaceList[i].keyList.Num() > 0)
				{
					//init : insert array index = 0
					tempKeyFaceList[i].sortIndexList.Add(0);
				}
				else
				{
					continue;
				}
				//sort all vmd-key frames
				for (int k = 1; k < tempKeyFaceList[i].keyList.Num(); k++)
				{
					bool isSetList = false;
					//TBD:2分探索で軽量に行くべきだが面倒くさいのでとりあえず、線形探索にする
					for (int s = 0; s < tempKeyFaceList[i].sortIndexList.Num(); s++)
					{
						//もし現在の位置よりも今回の値が大きいか？
						if (tempKeyFaceList[i].keyList[k].Frame <
							tempKeyFaceList[i].keyList[tempKeyFaceList[i].sortIndexList[s]].Frame)
						{
							/*if (s + 1 == tempKeyBoneList[i].sortIndexList.Num())
							{
							//insert array index = k;
							tempKeyBoneList[i].sortIndexList.Add(k);
							}
							else*/
							{
								//insert array index = k;
								tempKeyFaceList[i].sortIndexList.Insert(k, s);
							}
							//check list size == now index num
							//check(tempKeyBoneList[i].sortIndexList.Num() == k+1);
							isSetList = true;
							break;
						}
					}
					if (!isSetList)
					{
						//add last
						tempKeyFaceList[i].sortIndexList.Add(k);
					}
				}
			}
			keyFaceList = tempKeyFaceList;
		}
		if (readData->vmdCameraCount > 0)
		{
			//Keys
			TArray<VmdCameraTrackList>	tempKeyCamList;
			VmdCameraTrackList * vmdCamKeyTrackPtr = NULL;
			VMD_CAMERA * vmdCamKeyPtr = NULL;

			//Camera Key
			tempTrackNameList.Empty();
			arrayIndx = -1;
			tempKeyCamList.Empty();
			tempKeyCamList.Add(VmdCameraTrackList());//VMDにcamはひとつしかない為
			trackName = "MMDCamera000";//固定値
			//New Track
			vmdCamKeyTrackPtr = &(tempKeyCamList[0]);//VMDにcamはひとつしかない為
			vmdCamKeyTrackPtr->TrackName = trackName;
			for (int32 i = 0; i < readData->vmdCameraCount; i++)
			{
				// get ptr
				vmdCamKeyPtr = &(readData->vmdCameraList[i]);
				//

				check(vmdCamKeyTrackPtr);
				///
				arrayIndx = vmdCamKeyTrackPtr->keyList.Add(*vmdCamKeyPtr);
				//
				vmdCamKeyTrackPtr->maxFrameCount
					= FMath::Max(vmdCamKeyPtr->Frame, vmdCamKeyTrackPtr->maxFrameCount);
				vmdCamKeyTrackPtr->minFrameCount
					= FMath::Min(vmdCamKeyPtr->Frame, vmdCamKeyTrackPtr->minFrameCount);
				///
				maxFrame = FMath::Max(vmdCamKeyPtr->Frame, maxFrame);
				minFrame = FMath::Min(vmdCamKeyPtr->Frame, minFrame);
			}
			// Frame順に計算しやすいようにListのIndex参照をソートした配列を生成する。ただし無駄すぎる処理。VMDが順不同の為。
			for (int i = 0; i < tempKeyCamList.Num(); i++)
			{// all bone
				if (tempKeyCamList[i].keyList.Num() > 0)
				{
					//init : insert array index = 0
					tempKeyCamList[i].sortIndexList.Add(0);
				}
				else
				{
					continue;
				}
				//sort all vmd-key frames
				for (int k = 1; k < tempKeyCamList[i].keyList.Num(); k++)
				{
					bool isSetList = false;
					//TBD:2分探索で軽量に行くべきだが面倒くさいのでとりあえず、線形探索にする
					for (int s = 0; s < tempKeyCamList[i].sortIndexList.Num(); s++)
					{
						//もし現在の位置よりも今回の値が大きいか？
						if (tempKeyCamList[i].keyList[k].Frame <
							tempKeyCamList[i].keyList[tempKeyCamList[i].sortIndexList[s]].Frame)
						{
							/*if (s + 1 == tempKeyBoneList[i].sortIndexList.Num())
							{
							//insert array index = k;
							tempKeyBoneList[i].sortIndexList.Add(k);
							}
							else*/
							{
								//insert array index = k;
								tempKeyCamList[i].sortIndexList.Insert(k, s);
							}
							//check list size == now index num
							//check(tempKeyBoneList[i].sortIndexList.Num() == k+1);
							isSetList = true;
							break;
						}
					}
					if (!isSetList)
					{
						//add last
						tempKeyCamList[i].sortIndexList.Add(k);
					}
				}
			}
			keyCameraList = tempKeyCamList;
		}
		//////////////////////////////
		// end phase
		maxFrame++;			//min 1frame ?
		return true;
	}

	// 指定List内で該当するFrame名があればそのIndex値を返す。異常値=-1。
	int32 VmdMotionInfo::FindKeyTrackName(
		FString targetName,
		EVMDKEYFRAMETYPE listType)
	{
		int32  index = -1;
		//★TBD:VMDのボーン名が15Byte制限であり、前方一致で検索すべきだが、
		//面倒のため、完全一致型でボーン名尾（フレーム名)を検索する
		if (listType == EVMDKEYFRAMETYPE::EVMD_KEYBONE)
		{
			for (int i = 0; i < keyBoneList.Num(); i++)
			{
				if (keyBoneList[i].TrackName.Equals(targetName))
				{
					index = i;
					break;
				}
			}

		}
		else if (listType == EVMDKEYFRAMETYPE::EVMD_KEYFACE)
		{
			for (int i = 0; i < keyFaceList.Num(); i++)
			{
				if (keyFaceList[i].TrackName.Equals(targetName))
				{
					index = i;
					break;
				}
			}
		}
		else
		{
			//nop err
		}

		return index;
	}
}