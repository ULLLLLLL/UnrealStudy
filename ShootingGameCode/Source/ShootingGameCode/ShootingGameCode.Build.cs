// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShootingGameCode : ModuleRules
{
	public ShootingGameCode(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "UMG", "ItemPluginCode" }); // 클래스 모듈 확인하기

		PublicIncludePaths.AddRange(new string[] { "ShootingGameCode", "ShootingGameCode/Public" }); // 주소는 폴더에서 복사해온다.
	}
}