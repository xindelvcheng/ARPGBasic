// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TestProject : ModuleRules
{
	public TestProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "AIModule", "DeveloperSettings"
		});

		PrivateIncludePaths.AddRange(new string[]
		{
			"TestProject",
			"TestProject/Ability",
			"TestProject/Core",
			"TestProject/AI",
			"TestProject/Animation",
			"TestProject/Archive",
			"TestProject/Combat",
			"TestProject/GamePlay",
			"TestProject/GameItem",
			"TestProject/UI",	
		});
	}
}