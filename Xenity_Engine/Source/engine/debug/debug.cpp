#include "debug.h"
#include "../file_system/file_system.h"
#include "../network/network.h"

#if defined(__PSP__)
#include <pspkernel.h>
#include "../../psp/debug/debug.h"
#elif defined(__vita__)
#include "../../psvita/debug/debug.h"
#include <psp2/io/stat.h>
#endif
#include "../engine_settings.h"

File *file = nullptr;
std::string Debug::debugText = "";
Socket *Debug::socket;

/**
 * Print an error in the console and the debug file
 */
void Debug::PrintError(std::string text)
{
    if (!EngineSettings::useDebugger)
        return;

    PrintInOnlineConsole(text);
    text += '\n';
    std::string textWithoutColor = "[ERROR] " + text;
    std::string textWithColor = "\033[31m" + textWithoutColor;
    PrintInConsole(textWithColor);
    PrintInFile(textWithoutColor);
    debugText += text;
}

/**
 * Print a warning in the console and the debug file
 */
void Debug::PrintWarning(std::string text)
{
    if (!EngineSettings::useDebugger)
        return;

    PrintInOnlineConsole(text);
    text += '\n';
    std::string textWithoutColor = "[WARNING] " + text;
    std::string textWithColor = "\033[33m" + textWithoutColor;
    PrintInConsole(textWithColor);
    PrintInFile(textWithoutColor);
    debugText += text;
}

void Debug::PrintInConsole(std::string text)
{
#if defined(__PSP__)
    // PspDebugPrint(text);
#elif defined(__vita__)
    // PsVitaDebugPrint(text);
#else
    std::cout << text;
#endif
}

void Debug::PrintInFile(std::string text)
{
    if (file)
        file->Write(text);
}

/**
 * @brief Print text in the console and the debug file
 */
void Debug::Print(std::string text)
{
    if (!EngineSettings::useDebugger)
        return;

    PrintInOnlineConsole(text);
    text += '\n';
    std::string newString = "\033[37m" + text;
    PrintInConsole(newString);
    PrintInFile(text);
    debugText += text;
}

void Debug::PrintInOnlineConsole(std::string text)
{
    if (socket)
    {
        text += char(3);
        socket->SendData(text);
    }
}

void Debug::ConnectToOnlineConsole()
{
    Debug::Print("Connect to online console...");
    socket = NetworkManager::CreateSocket("88.127.205.17", 6004);
}

/**
 * @brief Init debug system (call once)
 *
 */
void Debug::Init()
{
    if (!EngineSettings::useDebugger)
        return;

    std::string fileName = "xenity_engine_debug.txt";
#if defined(__vita__)
    fileName = "data\\xenity_engine\\" + fileName;
#endif
    FileSystem::fileSystem->DeleteFile(fileName);
    file = new File(fileName);
    file->Open(true);

    Print("-------- Debug initiated --------");
}

void Debug::Update()
{
}
