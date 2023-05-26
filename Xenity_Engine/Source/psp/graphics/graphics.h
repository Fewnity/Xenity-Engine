#pragma once

#ifdef __PSP__

void CrossGraphicsInit();
void CrossGraphicsLoop();
void CrossGraphicsStop();
void CrossGraphicsSwapBuffer();

void PspStartFrame();
#endif