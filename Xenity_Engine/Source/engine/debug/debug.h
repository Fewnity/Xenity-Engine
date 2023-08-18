#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <iostream>

class Socket;

class API Debug
{
public:
    static int Init();
    static void Print(std::string text);
    static void PrintError(std::string text);
    static void PrintWarning(std::string text);
    static void ConnectToOnlineConsole();

    static std::string GetDebugString()
    {
        return debugText;
    }

private:
    static std::string debugText;
    static void PrintInOnlineConsole(std::string text);
    static void PrintInConsole(std::string text);
    static void PrintInFile(std::string text);
    static Socket *socket;
};