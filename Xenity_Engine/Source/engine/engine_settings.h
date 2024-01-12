#pragma once

/**
 * [Internal]
 */

#include <string>
#include <engine/reflection/reflection.h>
#include <engine/graphics/color/color.h>

class EngineSettings
{
public:

	/**
	* Save engine settings
	*/
	static void SaveEngineSettings();

	/**
	* Load engine settings
	*/
	static void LoadEngineSettings();

	static bool isWireframe;
	static int maxLightCount;
	static bool useProfiler;
	static bool useDebugger;
	static bool useOnlineDebugger;
	static std::string engineProjectPath;
	static std::string compilerPath;
	static std::string ppssppExePath;

	static bool compileOnCodeChanged;
	static bool compileWhenOpeningProject;

	static Color backbgroundColor;
	static Color secondaryColor;
	static Color playTintColor;
	static bool isPlayTintAdditive;

	static ReflectiveData GetReflectiveData();
};
