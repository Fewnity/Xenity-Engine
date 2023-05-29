#pragma once

#include <string>

class Texture
{
public:
    void Bind();
    void Load(const char *filename, const int vram);
    int width, height;

    void SetData(const unsigned char *data, int vram);

    // Specific to psp
    unsigned int pW, pH;
    void *data;
    int type;
    
    unsigned int id;
    std::string name = "";
};
