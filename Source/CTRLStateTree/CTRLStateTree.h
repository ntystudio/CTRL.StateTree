// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCTRLStateTree, Log, All);

#define CTRLST_LOG(Verbosity, ...) UE_LOG(LogCTRLStateTree, Verbosity, ##__VA_ARGS__)
#define CTRLST_CLOG(Cond, Verbosity, ...) UE_CLOG(Cond, LogCTRLStateTree, Verbosity, ##__VA_ARGS__)
#define CTRLST_VLOG(Subject, Verbosity, ...) UE_VLOG_UELOG(Subject, LogCTRLStateTree, Verbosity, ##__VA_ARGS__)

class FCTRLStateTreeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
