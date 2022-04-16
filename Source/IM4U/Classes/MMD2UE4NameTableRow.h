// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once

#include "Engine/DataTable.h"

#include "MMD2UE4NameTableRow.generated.h"

/**
* MMD系からUE4向けのデータに変換する際のテーブル定義
* -> VMD Import時に追加でこのテーブルを指定しMMD->UE4の翻訳として使う
* -> CSV でImportする際に日本語を使う場合はUTF-8で保存すること
* @param Name(デフォルトTag) : UE4側名称(ボーン名、Morphtarget名を記載すること)
* @param MMDOriginalName     : MMD側名称(ボーン名、表情名を入力すること)
*/
USTRUCT()
struct FMMD2UE4NameTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMD")
		FString MmdOriginalName;

};
