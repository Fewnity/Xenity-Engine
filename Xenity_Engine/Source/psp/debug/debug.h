#pragma once

#ifdef __PSP__

#include <string>

void PspDebugInit();
void PspDebugPrint(std::string text);

#endif