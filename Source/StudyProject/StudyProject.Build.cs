// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StudyProject : ModuleRules
{
	public StudyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "NetCore" });

		PublicIncludePaths.Add("StudyProject/");

		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities", 
            "GameplayTags", 
            "GameplayTasks"
		});
    }
}
