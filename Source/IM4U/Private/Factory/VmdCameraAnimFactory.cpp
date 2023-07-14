// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

/*
* Copyright 2010 Autodesk, Inc.  All Rights Reserved.
*
* Permission to use, copy, modify, and distribute this software in object
* code form for any purpose and without fee is hereby granted, provided
* that the above copyright notice appears in all copies and that both
* that copyright notice and the limited warranty and restricted rights
* notice below appear in all supporting documentation.
*
* AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS.
* AUTODESK SPECIFICALLY DISCLAIMS ANY AND ALL WARRANTIES, WHETHER EXPRESS
* OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTY
* OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE OR NON-INFRINGEMENT
* OF THIRD PARTY RIGHTS.  AUTODESK DOES NOT WARRANT THAT THE OPERATION
* OF THE PROGRAM WILL BE UNINTERRUPTED OR ERROR FREE.
*
* In no event shall Autodesk, Inc. be liable for any direct, indirect,
* incidental, special, exemplary, or consequential damages (including,
* but not limited to, procurement of substitute goods or services;
* loss of use, data, or profits; or business interruption) however caused
* and on any theory of liability, whether in contract, strict liability,
* or tort (including negligence or otherwise) arising in any way out
* of such code.
*
* This software is provided to the U.S. Government with the same rights
* and restrictions as described herein.
*/

// add : vmd to matinee camera for im4u plugin.
// Copyright 2015 BlackMa9. All Rights Reserved.

#include "../IM4UPrivatePCH.h"

//ここの関数を使ってVMDのカメラモーションをMatineeAnimにインポートさせたい予定
#if 1 //all

#include "CoreMinimal.h"
#include "Editor.h"


//#include "FbxImporter.h"
#include "Factory/VmdFactory.h"
#include "Camera/CameraActor.h"

//namespace TESTIM4U {
#if 0
	/**
	* Retrieves whether there are any unknown camera instances within the FBX document that the camera is not in Unreal scene.
	*/
	inline bool _HasUnknownCameras(AMatineeActor* InMatineeActor, FbxNode* Node, const TCHAR* Name)
	{
		FbxNodeAttribute* Attr = Node->GetNodeAttribute();
		if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eCamera)
		{
			// If we have a Matinee, try to name-match the node with a Matinee group name
			if (InMatineeActor != NULL && InMatineeActor->MatineeData != NULL)
			{
				UInterpGroupInst* GroupInst = InMatineeActor->FindFirstGroupInstByName(FString(Name));
				if (GroupInst != NULL)
				{
					AActor* GrActor = GroupInst->GetGroupActor();
					// Make sure we have an actor
					if (GrActor != NULL &&
						GrActor->IsA(ACameraActor::StaticClass()))
					{
						// OK, we found an existing camera!
						return false;
					}
				}
			}

			// Attempt to name-match the scene node for this camera with one of the actors.
			AActor* Actor = FindObject<AActor>(ANY_PACKAGE, Name);
			if (Actor == NULL || Actor->IsPendingKill())
			{
				return true;
			}
			else
			{
				// If you trigger this assertion, then you've got a name
				// clash between the FBX file and the level.
				check(Actor->IsA(ACameraActor::StaticClass()));
			}
		}

		return false;
	}

	bool UVmdFactory::HasUnknownCameras(AMatineeActor* InMatineeActor) const
	{
		if (Scene == NULL)
		{
			return false;
		}

		// check recursively
		FbxNode* RootNode = Scene->GetRootNode();
		int32 NodeCount = RootNode->GetChildCount();
		for (int32 NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
		{
			FbxNode* Node = RootNode->GetChild(NodeIndex);
			if (_HasUnknownCameras(InMatineeActor, Node, ANSI_TO_TCHAR(Node->GetName())))
			{
				return true;
			}

			// Look through children as well
			int32 ChildNodeCount = Node->GetChildCount();
			for (int32 ChildIndex = 0; ChildIndex < ChildNodeCount; ++ChildIndex)
			{
				FbxNode* ChildNode = Node->GetChild(ChildIndex);
				if (_HasUnknownCameras(InMatineeActor, ChildNode, ANSI_TO_TCHAR(ChildNode->GetName())))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool UVmdFactory::IsNodeAnimated(FbxNode* Node, FbxAnimLayer* AnimLayer)
	{
		if (!AnimLayer)
		{
			FbxAnimStack* AnimStack = Scene->GetMember<FbxAnimStack>(0);
			if (!AnimStack) return false;

			AnimLayer = AnimStack->GetMember<FbxAnimLayer>(0);
			if (AnimLayer == NULL) return false;
		}

		// verify that the node is animated.
		bool bIsAnimated = false;
		FbxTimeSpan AnimTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);

		// translation animation
		FbxProperty TransProp = Node->LclTranslation;
		for (int32 i = 0; i < TransProp.GetSrcObjectCount<FbxAnimCurveNode>(); i++)
		{
			FbxAnimCurveNode* CurveNode = TransProp.GetSrcObject<FbxAnimCurveNode>(i);
			if (CurveNode && AnimLayer->IsConnectedSrcObject(CurveNode))
			{
				bIsAnimated |= (bool)CurveNode->GetAnimationInterval(AnimTimeSpan);
				break;
			}
		}
		// rotation animation
		FbxProperty RotProp = Node->LclRotation;
		for (int32 i = 0; bIsAnimated == false && i < RotProp.GetSrcObjectCount<FbxAnimCurveNode>(); i++)
		{
			FbxAnimCurveNode* CurveNode = RotProp.GetSrcObject<FbxAnimCurveNode>(i);
			if (CurveNode && AnimLayer->IsConnectedSrcObject(CurveNode))
			{
				bIsAnimated |= (bool)CurveNode->GetAnimationInterval(AnimTimeSpan);
			}
		}

		return bIsAnimated;
	}

	/**
	* Finds a camera in the passed in node or any child nodes
	* @return NULL if the camera is not found, a valid pointer if it is
	*/
	static FbxCamera* FindCamera(FbxNode* Parent)
	{
		FbxCamera* Camera = Parent->GetCamera();
		if (!Camera)
		{
			int32 NodeCount = Parent->GetChildCount();
			for (int32 NodeIndex = 0; NodeIndex < NodeCount && !Camera; ++NodeIndex)
			{
				FbxNode* Child = Parent->GetChild(NodeIndex);
				Camera = Child->GetCamera();
			}
		}

		return Camera;
	}
#endif




	//この関数の戻り値はMMDのカーブ特性に合わせて固定値を変えさせるように修正する
	EInterpCurveMode UVmdFactory::GetUnrealInterpMode(
		//FbxAnimCurveKey FbxKey
		)
	{
		//TBD::固定値に検討に関してはカーブ特性を調査すること->暫定でCurveAutoClampedにする。
		EInterpCurveMode Mode = CIM_CurveAutoClamped;

#ifdef ORIGINAL_FBX_MATINEE_IMPORT
		EInterpCurveMode Mode = CIM_CurveUser;

		// Convert the interpolation type from FBX to Unreal.
		switch (FbxKey.GetInterpolation())
		{
		case FbxAnimCurveDef::eInterpolationCubic:
		{
			switch (FbxKey.GetTangentMode())
			{
				// Auto tangents will now be imported as user tangents to allow the
				// user to modify them without inadvertently resetting other tangents
				// 				case KFbxAnimCurveDef::eTANGENT_AUTO:
				// 					if ((KFbxAnimCurveDef::eTANGENT_GENERIC_CLAMP & FbxKey.GetTangentMode(true)))
				// 					{
				// 						Mode = CIM_CurveAutoClamped;
				// 					}
				// 					else
				// 					{
				// 						Mode = CIM_CurveAuto;
				// 					}
				// 					break;
			case FbxAnimCurveDef::eTangentBreak:
				Mode = CIM_CurveBreak;
				break;
			case FbxAnimCurveDef::eTangentAuto:
				Mode = CIM_CurveAuto;
				break;
			case FbxAnimCurveDef::eTangentUser:
			case FbxAnimCurveDef::eTangentTCB:
				Mode = CIM_CurveUser;
				break;
			default:
				break;
			}
			break;
		}

		case FbxAnimCurveDef::eInterpolationConstant:
			if (FbxKey.GetTangentMode() != (FbxAnimCurveDef::ETangentMode)FbxAnimCurveDef::eConstantStandard)
			{
				// warning not support
				;
			}
			Mode = CIM_Constant;
			break;

		case FbxAnimCurveDef::eInterpolationLinear:
			Mode = CIM_Linear;
			break;
		}
#endif
		return Mode;
	}
	
#if 0 //test build 2
	//この関数が必要か不明。SubTrackの存在について調査が必要。
	void UVmdFactory::ImportMoveSubTrack(
//		FbxAnimCurve* FbxCurve, 
		int32 FbxDimension, 
		UInterpTrackMoveAxis* SubTrack,
		int32 CurveIndex, 
		bool bNegative, 
//		FbxAnimCurve* RealCurve, 
		float DefaultVal
		)
	{
		if (CurveIndex >= 3) return;

		FInterpCurveFloat& Curve = SubTrack->FloatTrack;
		// the FBX curve has no valid keys, so fake the Unreal Matinee curve
		if (FbxCurve == NULL || FbxCurve->KeyGetCount() < 2)
		{
			int32 KeyIndex;
			for (KeyIndex = Curve.Points.Num(); KeyIndex < RealCurve->KeyGetCount(); ++KeyIndex)
			{
				float Time = (float)RealCurve->KeyGet(KeyIndex).GetTime().GetSecondDouble();
				// Create the curve keys
				FInterpCurvePoint<float> Key;
				Key.InVal = Time;
				Key.InterpMode = GetUnrealInterpMode(RealCurve->KeyGet(KeyIndex));

				Curve.Points.Add(Key);
			}

			for (KeyIndex = 0; KeyIndex < RealCurve->KeyGetCount(); ++KeyIndex)
			{
				FInterpCurvePoint<float>& Key = Curve.Points[KeyIndex];
				Key.OutVal = DefaultVal;
				Key.ArriveTangent = 0;
				Key.LeaveTangent = 0;
			}
		}
		else
		{
			int32 KeyCount = (int32)FbxCurve->KeyGetCount();

			for (int32 KeyIndex = Curve.Points.Num(); KeyIndex < KeyCount; ++KeyIndex)
			{
				FbxAnimCurveKey CurKey = FbxCurve->KeyGet(KeyIndex);

				// Create the curve keys
				FInterpCurvePoint<float> Key;
				Key.InVal = CurKey.GetTime().GetSecondDouble();

				Key.InterpMode = GetUnrealInterpMode(CurKey);

				// Add this new key to the curve
				Curve.Points.Add(Key);
			}

			// Fill in the curve keys with the correct data for this dimension.
			for (int32 KeyIndex = 0; KeyIndex < KeyCount; ++KeyIndex)
			{
				FbxAnimCurveKey CurKey = FbxCurve->KeyGet(KeyIndex);
				FInterpCurvePoint<float>& UnrealKey = Curve.Points[KeyIndex];

				// Prepare the FBX values to import into the track key.
				// Convert the Bezier control points, if available, into Hermite tangents
				float OutVal = bNegative ? -CurKey.GetValue() : CurKey.GetValue();

				float ArriveTangent = 0.0f;
				float LeaveTangent = 0.0f;

				if (CurKey.GetInterpolation() == FbxAnimCurveDef::eInterpolationCubic)
				{
					ArriveTangent = bNegative ? -FbxCurve->KeyGetLeftDerivative(KeyIndex) : FbxCurve->KeyGetLeftDerivative(KeyIndex);
					LeaveTangent = bNegative ? -FbxCurve->KeyGetRightDerivative(KeyIndex) : FbxCurve->KeyGetRightDerivative(KeyIndex);
				}

				// Fill in the track key with the prepared values
				UnrealKey.OutVal = OutVal;
				UnrealKey.ArriveTangent = ArriveTangent;
				UnrealKey.LeaveTangent = LeaveTangent;
			}
		}
	}
#endif

	//この関数が必要か不明。MatineeAnimatedがどの部分に該当するかについて調査が必要。
	void UVmdFactory::ImportMatineeAnimated(
		MMD4UE4::VmdCameraTrackList * VmdCurve,
		//FbxAnimCurve* FbxCurve,
		FInterpCurveVector& Curve, 
		int32 CurveIndex, 
		bool bNegative, 
		//FbxAnimCurve* RealCurve,
		float DefaultVal
		)
	{
		if (CurveIndex >= 3) return;

		// the FBX curve has no valid keys, so fake the Unreal Matinee curve
#if 0 //test
		if (FbxCurve == NULL || FbxCurve->KeyGetCount() < 2)
		{
			int32 KeyIndex;
			for (KeyIndex = Curve.Points.Num(); KeyIndex < RealCurve->KeyGetCount(); ++KeyIndex)
			{
				float Time = (float)RealCurve->KeyGet(KeyIndex).GetTime().GetSecondDouble();
				// Create the curve keys
				FInterpCurvePoint<FVector> Key;
				Key.InVal = Time;
				Key.InterpMode = GetUnrealInterpMode(RealCurve->KeyGet(KeyIndex));

				Curve.Points.Add(Key);
			}

			for (KeyIndex = 0; KeyIndex < RealCurve->KeyGetCount(); ++KeyIndex)
			{
				FInterpCurvePoint<FVector>& Key = Curve.Points[KeyIndex];
				switch (CurveIndex)
				{
				case 0:
					Key.OutVal.X = DefaultVal;
					Key.ArriveTangent.X = 0;
					Key.LeaveTangent.X = 0;
					break;
				case 1:
					Key.OutVal.Y = DefaultVal;
					Key.ArriveTangent.Y = 0;
					Key.LeaveTangent.Y = 0;
					break;
				case 2:
				default:
					Key.OutVal.Z = DefaultVal;
					Key.ArriveTangent.Z = 0;
					Key.LeaveTangent.Z = 0;
					break;
				}
			}
		}
		else
#endif
		{
			int32 KeyCount = (int32)VmdCurve->keyList.Num();

			for (int32 KeyIndex = Curve.Points.Num(); KeyIndex < KeyCount; ++KeyIndex)
			{
				//FbxAnimCurveKey CurKey = FbxCurve->KeyGet(KeyIndex);
				MMD4UE4::VMD_CAMERA* CurKeyPtr = & VmdCurve->keyList[VmdCurve->sortIndexList[KeyIndex]];

				// Create the curve keys
				FInterpCurvePoint<FVector> Key;
				//Key.InVal = CurKey.GetTime().GetSecondDouble();
				Key.InVal = (float)(CurKeyPtr->Frame) * this->baseFrameRate;

				Key.InterpMode = GetUnrealInterpMode(/*CurKey*/);

				// Add this new key to the curve
				Curve.Points.Add(Key);
			}

			// Fill in the curve keys with the correct data for this dimension.
			for (int32 KeyIndex = 0; KeyIndex < KeyCount; ++KeyIndex)
			{
				//FbxAnimCurveKey CurKey = FbxCurve->KeyGet(KeyIndex);
				MMD4UE4::VMD_CAMERA* CurKeyPtr = & VmdCurve->keyList[VmdCurve->sortIndexList[KeyIndex]];
				FInterpCurvePoint<FVector>& UnrealKey = Curve.Points[KeyIndex];

				// Prepare the FBX values to import into the track key.
				// Convert the Bezier control points, if available, into Hermite tangents
				//float OutVal = (bNegative ? -CurKey.GetValue() : CurKey.GetValue());
				
				float OutVal = CurKeyPtr->Location[CurveIndex];

				float ArriveTangent = 0.0f;
				float LeaveTangent = 0.0f;

				/* fbx
				if (CurKey.GetInterpolation() == FbxAnimCurveDef::eInterpolationCubic)
				{
					ArriveTangent = bNegative ? -FbxCurve->KeyGetLeftDerivative(KeyIndex) : FbxCurve->KeyGetLeftDerivative(KeyIndex);
					LeaveTangent = bNegative ? -FbxCurve->KeyGetRightDerivative(KeyIndex) : FbxCurve->KeyGetRightDerivative(KeyIndex);
				}*/

				// Fill in the track key with the prepared values
				switch (CurveIndex)
				{
				case 0:
					UnrealKey.OutVal.X = OutVal;
					UnrealKey.ArriveTangent.X = ArriveTangent;
					UnrealKey.LeaveTangent.X = LeaveTangent;
					break;
				case 1:
					UnrealKey.OutVal.Y = OutVal;
					UnrealKey.ArriveTangent.Y = ArriveTangent;
					UnrealKey.LeaveTangent.Y = LeaveTangent;
					break;
				case 2:
				default:
					UnrealKey.OutVal.Z = OutVal;
					UnrealKey.ArriveTangent.Z = ArriveTangent;
					UnrealKey.LeaveTangent.Z = LeaveTangent;
					break;
				}
			}
		}
	}

//} // namespace UnFBX


#endif