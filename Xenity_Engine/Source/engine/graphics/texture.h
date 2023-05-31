#pragma once

#include <string>

class Texture
{
public:
    void Bind();
    void Load(const char *filename, const int vram);
    int width, height;

    void SetData(const unsigned char *data, int vram, bool needResize);

#ifdef __PSP__
    unsigned int pW = 0;
    unsigned int pH = 0;
    void *data = nullptr;
    int type;
#endif

    unsigned int id;
    std::string name = "";
};
