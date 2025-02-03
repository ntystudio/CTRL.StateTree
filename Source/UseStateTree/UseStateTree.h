// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUseStateTree, Log, All);

#define UST_LOG(Verbosity, ...) UE_LOG(LogUseStateTree, Verbosity, ##__VA_ARGS__)
#define UST_CLOG(Cond, Verbosity, ...) UE_CLOG(Cond, LogUseStateTree, Verbosity, ##__VA_ARGS__)
#define UST_VLOG(Subject, Verbosity, ...) UE_VLOG_UELOG(Subject, LogUseStateTree, Verbosity, ##__VA_ARGS__)

class FUseStateTreeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
