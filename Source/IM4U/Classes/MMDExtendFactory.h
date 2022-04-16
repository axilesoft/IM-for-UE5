// Copyright 2015 BlackMa9. All Rights Reserved.
#pragma once

#include "Factories/Factory.h"
#include "MMDExtendFactory.generated.h"


/****************************************
* MMD Extend Asset for Create New ( factory )
*****************************************/
UCLASS(HideCategories = Object, MinimalAPI)
class UMMDExtendFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual bool DoesSupportClass(UClass* Class) override;
	virtual UClass* ResolveSupportedClass() override;

	virtual UObject* FactoryCreateNew(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn
		) override;
};