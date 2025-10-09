// By hzFishy - 2025 - Do whatever you want with it.

using UnrealBuildTool;

public class CommonAIBehaviors : ModuleRules
{
	public CommonAIBehaviors(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "Engine", "CoreUObject",
			"GameplayTags",
			"AIModule", "StateTreeModule",
			"FishyUtils"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", "SlateCore",
		});
	}
}
