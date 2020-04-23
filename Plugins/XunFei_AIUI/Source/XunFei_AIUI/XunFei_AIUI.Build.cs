// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
public class XunFei_AIUI : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string XunFeiPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../XunFei/")); }
        //get { return ; }

    }

    private string GetUProjectBinaryPath()
    {
        return Path.GetFullPath(Path.Combine(ModulePath, "../../../../Binaries/"));
    }

    private void CopyToBinaries(string Filepath, /*TargetInfo*/ReadOnlyTargetRules Target)
    {
        //string binariesDir = Path.Combine(GetUProjectPath(), "Binaries", Target.Platform.ToString());
        string binariesDir = Path.Combine(GetUProjectBinaryPath(), Target.Platform.ToString());

        string filename = Path.GetFileName(Filepath);

        if (!Directory.Exists(binariesDir))
            Directory.CreateDirectory(binariesDir);

        if (!File.Exists(Path.Combine(binariesDir, filename)))
        File.Copy(Filepath, Path.Combine(binariesDir, filename), true);
    }

    public XunFei_AIUI(ReadOnlyTargetRules Target) : base(Target)
	{

        // We want to distribute binaries-only, not source.
        bOutputPubliclyDistributable = true;

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableUndefinedIdentifierWarnings = false;
        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "InputCore",
                "Json",
                "Projects",
                "UMG",
                "Slate",
                "SlateCore",

				// ... add other public dependencies that you statically link with here ...
			}
			);

        PrivateIncludePaths.Add("XunFei_iat/Private");
        PublicIncludePaths.Add("XunFei_iat/Public");

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.AddRange(
                new string[]
                {
                    Path.Combine(XunFeiPath, "include"),
                   
                }
            );
            //PublicLibraryPaths.AddRange(
            //    new string[]
            //    {
            //        Path.Combine(XunFeiPath, "libs"),
            //    }
            //);

            string libPath = Path.Combine(XunFeiPath, "libs");

            PublicAdditionalLibraries.AddRange(new string[] { Path.Combine(libPath,"msc_x64.lib") });
            PublicAdditionalLibraries.AddRange(new string[] { Path.Combine(libPath, "aiui_x64.lib") });

            //RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(XunFeiPath, "msc_x64.dll")));
            CopyToBinaries(Path.Combine(XunFeiPath, "bin", "aiui_x64.dll"), Target);
        }

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
