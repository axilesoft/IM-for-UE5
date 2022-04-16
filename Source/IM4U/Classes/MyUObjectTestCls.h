// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once

#include "MyUObjectTestCls.generated.h"


struct SPMDHeader
{
	char magic[3];//PMD
	float ver;
	char name[20];
	char comment[256];
};


struct SPMXHeader
{
	//char magic[3];//PMD
	char magic[4];//PMX
	//char ver[4];//PMX
	float ver;
	uint8 byteSize;
	uint8 byteDate[8];
	//uint16 nameJPSize; 
	//uint32 nameJPSize;
	TCHAR nameJP[256];
	//uint32 nameEngSize;
	TCHAR nameEng[256];
	//uint32 CommentJPSize;
	TCHAR CommentJP[256];
	//uint32 CommentEngSize;
	TCHAR CommentEng[256];
};

/**
* Example UStruct declared in a plugin module
*/
USTRUCT()
struct FMyPluginStructTEST
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = MyPluginStructTEST)
		FString FormatTYPE;

	UPROPERTY(EditAnywhere, Category = MyPluginStructTEST)
		float FormatVersion;

	UPROPERTY(EditAnywhere, Category = MyPluginStructTEST)
		FString ModelName;

	UPROPERTY(EditAnywhere, Category = MyPluginStructTEST)
		FString ModelNameEng;

	UPROPERTY(EditAnywhere, Category = MyPluginStructTEST)
		FString ModelComment;

	UPROPERTY(EditAnywhere, Category = MyPluginStructTEST)
		FString ModelCommentEng;
};

/**
* Example of declaring a UObject in a plugin module
*/
UCLASS(hidecategories = Object)
class UMyUObjectTestCls : public UObject
{
	GENERATED_UCLASS_BODY()

public:


	UPROPERTY(EditAnywhere, Category = MyUObjectTestCls)
	struct FMyPluginStructTEST MyStruct;
private:

};
