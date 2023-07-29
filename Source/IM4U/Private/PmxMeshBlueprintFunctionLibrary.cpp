// Fill out your copyright notice in the Description page of Project Settings.


#include "PmxMeshBlueprintFunctionLibrary.h"

#include "Dataflow/DataflowEngineUtil.h"
#include "Engine/SkeletalMesh.h"
#include "Dataflow/DataflowInputOutput.h"
#include "GeometryCollection/Facades/CollectionConstraintOverrideFacade.h"
#include "GeometryCollection/Facades/CollectionKinematicBindingFacade.h"
#include "Dataflow/DataflowNodeFactory.h"
#include "GeometryCollection/Facades/CollectionVertexBoneWeightsFacade.h"
#include "GeometryCollection/TransformCollection.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "BoneWeights.h"

void UPmxMeshBlueprintFunctionLibrary::MyStaticFunction(USkeletalMeshComponent* meshCom)
{
    // Implement your logic here
    // This function will be callable from Blueprints
    UE_LOG(LogTemp, Warning, TEXT("MyStaticFunction was called from Blueprint!"));
    if (!meshCom) return;

    if (UPhysicsAsset* OriginalPhysicsAsset = meshCom->GetPhysicsAsset())
    {
        
            // Clone the physics asset to make changes without affecting the original asset
            UPhysicsAsset* ModifiedPhysicsAsset = DuplicateObject<UPhysicsAsset>(OriginalPhysicsAsset, nullptr);

            TArray<TObjectPtr<USkeletalBodySetup>> SkeletalBodySetups = ModifiedPhysicsAsset->SkeletalBodySetups;

            for (TObjectPtr<USkeletalBodySetup> &BodySetup : SkeletalBodySetups)
            {
                BodySetup->PhysicsType = EPhysicsType::PhysType_Default;
                // ... other modifications as needed
            }

            // Set the modified physics asset on the SkeletalMeshComponent
            meshCom->SetPhysicsAsset(ModifiedPhysicsAsset);

            // Recreate the physics state to apply the changes
            meshCom->RecreatePhysicsState();
        
    }
}