#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

class API InputPad
{
public:
    int buttons = 0;
    float lx = 0;
    float ly = 0;
    float rx = 0;
    float ry = 0;
};