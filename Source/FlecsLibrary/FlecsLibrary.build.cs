using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
	public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"FlecsTest",
		});

		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {"FlecsLibrary/Public"});
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {"FlecsLibrary/Private"});
		//UBT has no idea what 'flecs_EXPORTS' is
		//AppendStringToPublicDefinition("flecs_EXPORTS", "0");
		AppendStringToPublicDefinition("HAVE_EXECINFO", "0");
		
	}
}