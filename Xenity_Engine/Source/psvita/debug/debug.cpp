#include "debug.h"

#ifdef __vita__

#include "../common/debugScreen.h"

/**
 * @brief Write text in the debug file
 */
void PsVitaDebugPrint(std::string text)
{
	psvDebugScreenPrintf(text.c_str());
}

/**
 * @brief Init debug system (call once)
 *
 */
void PsVitaDebugInit()
{
	psvDebugScreenInit();
	psvDebugScreenSetFont(psvDebugScreenScaleFont2x(psvDebugScreenGetFont()));
}
#endif