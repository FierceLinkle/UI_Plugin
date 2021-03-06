// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TAM_CW : ModuleRules
{
	public TAM_CW(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "AIModule", "UIPlugin" });
	}
}
