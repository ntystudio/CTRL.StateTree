// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

using UnrealBuildTool;

public class CTRLStateTree : ModuleRules
{
	public CTRLStateTree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CommonUI",
				"FieldNotification",
				"StateTreeModule",
				"GameplayStateTreeModule",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AIModule",
				"CommonUI",
				"CoreUObject",
				"CTRLCore",
				"Engine",
				"GameplayAbilities",
				"GameplayStateTreeModule",
				"GameplayTags",
				"ModelViewViewModelBlueprint",
				"Slate",
				"SlateCore",
				"StateTreeModule",
				"UMG",
			}
		);
	}
}