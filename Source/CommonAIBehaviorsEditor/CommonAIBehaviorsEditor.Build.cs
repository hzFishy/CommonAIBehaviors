// By hzFishy - 2025 - Do whatever you want with it.

using UnrealBuildTool;

public class CommonAIBehaviorsEditor : ModuleRules
{
    public CommonAIBehaviorsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine",
            "Slate", "SlateCore",
            "InputCore",
            "DetailCustomizations", "PropertyEditor", "UnrealEd", "EditorStyle", "ComponentVisualizers",
            "CommonAIBehaviors",
        });

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
            
        });
    }
}