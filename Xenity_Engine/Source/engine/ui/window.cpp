#include "window.h"

int Window::width = 0;
int Window::height = 0;
float Window::aspect = 0;

void Window::SetResolution(const int width_, const int height_)
{
    width = width_;
    height = height_;
    UpdateAspectRatio();
}

int Window::GetWidth()
{
    return width;
}

int Window::GetHeight()
{
    return height;
}

float Window::GetAspectRatio()
{
    return aspect;
}

void Window::UpdateAspectRatio()
{
    aspect = width / height;
}