
// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once


#include "Engine.h"



namespace MMD4UE4
{
	// ToDo: MMD(PMX,PMD,VMD)の共通関数としてリファクタリングする事
	//		 現状、PMX系、PMD/VMD系で別れて実装している為。

	///////////////
	// encode type 
	///////////////
	enum PMXEncodeType
	{
		PMXEncodeType_UTF16LE = 0,	//for PMX
		PMXEncodeType_UTF8,			//for PMX
		PMXEncodeType_SJIS,			//for PMD, VMD
		PMXEncodeType_ERROR
	};
	class MMDImportHelper
	{
	public:
		//////////////////////////////////////
		// from MMD Asix To UE4 Asix 
		// param  : vec , in vector
		// return : convert vec ,out
		//////////////////////////////////////
		FVector3f ConvertVectorAsixToUE4FromMMD(
			FVector3f vec
			);
		//////////////////////////////////////
		// from PMX Binary Buffer To String @ TextBuf
		// 4 + n: TexBuf
		// buf : top string (top data)
		// encodeType : 0 utf-16, 1 utf-8
		//////////////////////////////////////
		FString PMXTexBufferToFString(
			const uint8 ** buffer,
			PMXEncodeType encodeType
			);
		//////////////////////////////////////
		// from MMD char (SJIS) To FString 
		// 4 + n: TexBuf
		// buf : top string (top data)
		// encodeType : SJIS 
		// for Pmd , Vmd format
		//////////////////////////////////////
		FString ConvertMMDSJISToFString(
			uint8 * buffer,
			const uint32 size
			);

		/////////////////////////////////////
		// import uint
		//////////////////////////////////////
		uint32 MMDExtendBufferSizeToUint32(
			const uint8 ** buffer,
			const uint8  blockSize, uint32 offset=0
			);
		/////////////////////////////////////
		// import int
		//////////////////////////////////////
		int32 MMDExtendBufferSizeToInt32(
			const uint8 ** buffer,
			const uint8  blockSize
			);
	};
}