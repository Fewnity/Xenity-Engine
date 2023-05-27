#pragma once

#include <string>

class Texture
{
public:
    void Bind();
    void Load(const char *filename, const int vram);
    int width, height;

    // Specific to psp
    unsigned int pW, pH;
    void *data;

    unsigned int id;
    std::string name = "";
};
