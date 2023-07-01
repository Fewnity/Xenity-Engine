#pragma once

#include <fstream>
#include <string>
#include <iostream>

class Socket;

class Debug
{
public:
    static void Init();
    static void Print(std::string text);
    static void PrintError(std::string text);
    static void PrintWarning(std::string text);
    static void PrintInOnlineConsole(std::string text);
    static void Update();
    static void ConnectToOnlineConsole();

    static std::string GetDebugString() 
    {
        return debugText;
    }

private:
    static std::ofstream debugFile;
    static std::string debugText;
    static void PrintInConsole(std::string text);
    static void PrintInFile(std::string text);
    static Socket *socket;
};