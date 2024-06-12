using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
	public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {"FlecsLibrary/Public"});
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {"FlecsLibrary/Private"});

		if (Target.Platform != UnrealTargetPlatform.Win64)
			AppendStringToPublicDefinition("flecs_EXPORTS", "0");
	}
}