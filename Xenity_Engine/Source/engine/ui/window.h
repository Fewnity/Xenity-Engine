#pragma once

class Window
{
public:
    static void SetResolution(const int width_, const int height_);
    static int GetWidth();
    static int GetHeight();
    static float GetAspectRatio();

private:
    static void UpdateAspectRatio();
    static int width, height;
    static float aspect;
};