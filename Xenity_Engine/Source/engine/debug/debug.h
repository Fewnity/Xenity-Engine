#pragma once

#include <fstream>
#include <string>
#include <iostream>

class Socket;

class Debug
{
public:
    static void Init();
    static std::ofstream debugFile;
    static void Print(std::string text);
    static void PrintError(std::string text);
    static void PrintWarning(std::string text);
    static void PrintInOnlineConsole(std::string text);
    static std::string debugText;
    static void Update();
    static void ConnectToOnlineConsole();

private:
    static void PrintInConsole(std::string text);
    static void PrintInFile(std::string text);
    static Socket *socket;
};