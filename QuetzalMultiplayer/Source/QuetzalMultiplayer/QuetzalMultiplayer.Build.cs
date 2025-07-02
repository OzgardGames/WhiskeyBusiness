// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class QuetzalMultiplayer : ModuleRules
{
	public QuetzalMultiplayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore", "GeometryCollectionEngine"});

        PublicIncludePaths.AddRange(new string[] { 
			"QuetzalMultiplayer", 
			"QuetzalMultiplayer/Actors", 
			"QuetzalMultiplayer/Actors/Character",
            "QuetzalMultiplayer/Actors/Character/Animation",
            "QuetzalMultiplayer/Actors/NPC",
            "QuetzalMultiplayer/MapObjects",
            "QuetzalMultiplayer/Components/PickupObjectsComp",
            "QuetzalMultiplayer/Components/DamageSystemComp",
            "QuetzalMultiplayer/Components/SpawnerComp",
            "QuetzalMultiplayer/HUD", 
			"QuetzalMultiplayer/Weapons", 
			"QuetzalMultiplayer/Weapons/Melee", 
			"QuetzalMultiplayer/Weapons/Ranged", 
			"QuetzalMultiplayer/Weapons/Ranged/Projectiles",
            "QuetzalMultiplayer/Minigames",
            "QuetzalMultiplayer/Minigames/MinigamesWidgets"     });
    }
}
