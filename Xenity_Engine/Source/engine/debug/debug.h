#pragma once

#include <fstream>
#include <string>
#include <iostream>

class Debug
{
public:
    static void Init();
    static std::ofstream debugFile;
    static void Print(std::string text);
    static void PrintError(std::string text);
    static void PrintWarning(std::string text);

private:
};