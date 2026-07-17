// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MGS_Project_First : ModuleRules
{
    public MGS_Project_First(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Retained AnimGraphRuntime for your C2039 fix, AND added UMG for the UI Crosshair.
        // Added Niagara for Combat VFX support.
        // ADDED AIModule for EnemyAIController perception and blackboard support.
        // ADDED NavigationSystem for C++ NavMesh queries.
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AnimGraphRuntime", "EnhancedInput", "UMG", "Niagara", "AIModule", "NavigationSystem" });

        // Uncommented and added Slate and SlateCore for deep UI framework support
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}