// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExtendedStateTree, Log, All);

#define EST_LOG(Verbosity, ...) UE_LOG(LogExtendedStateTree, Verbosity, ##__VA_ARGS__)
#define EST_CLOG(Cond, Verbosity, ...) UE_CLOG(Cond, LogExtendedStateTree, Verbosity, ##__VA_ARGS__)
#define EST_VLOG(Subject, Verbosity, ...) UE_VLOG_UELOG(Subject, LogExtendedStateTree, Verbosity, ##__VA_ARGS__)

class FExtendedStateTreeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
