// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class netherciv : ModuleRules
{
	public netherciv(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        //ONLINE RESOURCE SAYS TO USE PUBLIC. TRY THIS IF THIS DOESNT WORK.
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem" });

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
