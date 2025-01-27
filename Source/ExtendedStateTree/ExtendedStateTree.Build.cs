// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExtendedStateTree : ModuleRules
{
	public ExtendedStateTree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CommonUI",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"CommonUI",
				"Engine",
				"GameplayTags",
				"GameplayAbilities",
				"GameplayStateTreeModule",
				"Slate",
				"SlateCore",
				"StateTreeModule",
				"UMG",
			}
			);
	}
}
