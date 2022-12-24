// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(FlecsLibrary, All, All);

class FFlecsLibraryModule : public IModuleInterface
{
public:
 
	/* This will get called when the editor loads the module */
	virtual void StartupModule() override;
 
	/* This will get called when the editor unloads the module */
	virtual void ShutdownModule() override;
};
