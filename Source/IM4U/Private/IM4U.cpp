// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "IM4UPrivatePCH.h"


class FIM4U : public IIM4U
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FIM4U, IM4U )



void FIM4U::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FIM4U::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



