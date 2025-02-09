#pragma once

#include <string>

/**
* @brief The ScreenCapture class allows to make a screenshot of the game
*/
class ScreenCapture
{
public:
	/**
	* @brief Make a screenshot of the game (.png)
	* @brief (Note: on PSP/PsVita, plugins overlays are also captured)
	* 
	* @param fileName The name of the file to save the screenshot (without the extension)
	*/
	static bool MakeScreenshot(const std::string& fileName);
};

