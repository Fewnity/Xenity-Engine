#include "debug.h"
#include "../file_system/file_system.h"
#include "../network/network.h"

#if defined(__PSP__)
#include <pspkernel.h>
#include "../../psp/debug/debug.h"
File *file = nullptr;
#elif defined(__vita__)
#include "../../psvita/debug/debug.h"
#include <psp2/io/stat.h>
#endif

std::ofstream Debug::debugFile;
std::string Debug::debugText = "";
Socket *Debug::socket;

/**
 * Print an error in the console and the debug file
 */
void Debug::PrintError(std::string text)
{
    PrintInOnlineConsole(text);
    text += '\n';
    std::string newString = "\033[31m[ERROR] " + text;
    PrintInConsole(newString);
    PrintInFile(text);
    debugText += text;
}

/**
 * Print a warning in the console and the debug file
 */
void Debug::PrintWarning(std::string text)
{
    PrintInOnlineConsole(text);
    text += '\n';
    std::string newString = "\033[33m[WARNING] " + text;
    PrintInConsole(newString);
    PrintInFile(text);
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
#if defined(__PSP__)
    if (file)
        file->Write(text);
#elif defined(__vita__)
    debugFile << text;
    debugFile.flush();
#else
    debugFile << text;
    debugFile.flush();
#endif
}

/**
 * @brief Print text in the console and the debug file
 */
void Debug::Print(std::string text)
{
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
#if defined(__PSP__)
    // PspDebugInit();
    // Delete old debug file
    FileSystem::fileSystem->DeleteFile("xenity_engine_debug.txt");
    // Init debug file
    file = new File("xenity_engine_debug.txt");
#elif defined(__vita__)
    // PsVitaDebugInit();
    //  Create folder
    sceIoMkdir("ux0:/data/xenity_engine", 0777);
    // Init debug file
    debugFile.open("ux0:data/xenity_engine/xenity_engine_debug.txt");
#else
    // Init debug file
    debugFile.open("xenity_engine_debug.txt");

#endif
    Print("-------- Debug initiated --------");
}

void Debug::Update()
{
}
