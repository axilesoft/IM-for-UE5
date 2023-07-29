// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PmxMeshBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class IM4U_API UPmxMeshBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    // Declare the static function that you want to call in Blueprints
    UFUNCTION(BlueprintCallable, Category = "MyCategory")
        static void MyStaticFunction(USkeletalMeshComponent *meshCom);
};
