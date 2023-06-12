#include "debug.h"
#include "../file_system/file_system.h"

#ifdef __PSP__
#include <pspkernel.h>
#include "../../psp/debug/debug.h"
File *file = nullptr;
#elif __vita__
#include "../../psvita/debug/debug.h"
#include <psp2/io/stat.h>
#endif

std::ofstream Debug::debugFile;

/**
 * Print an error in the console and the debug file
 */
void Debug::PrintError(std::string text)
{
    std::string newString = "[ERROR] " + text;
    Print(newString);
}

/**
 * Print a warning in the console and the debug file
 */
void Debug::PrintWarning(std::string text)
{
    std::string newString = "[WARNING] " + text;
    Print(newString);
}

/**
 * @brief Print text in the console and the debug file
 */
void Debug::Print(std::string text)
{
    text += '\n';
#ifdef __PSP__
    // PspDebugPrint(text);
    if (file)
        file->Write(text);
#elif __vita__
    // PsVitaDebugPrint(text);
    debugFile << text;
    debugFile.flush();
#else
    std::cout << text;
    debugFile << text;
    debugFile.flush();
#endif
}

/**
 * @brief Init debug system (call once)
 *
 */
void Debug::Init()
{
#ifdef __PSP__
    // PspDebugInit();
    // Delete old debug file
    FileSystem::fileSystem->DeleteFile("xenity_engine_debug.txt");
    // Init debug file
    file = new File("xenity_engine_debug.txt");
#elif __vita__
    // PsVitaDebugInit();
    // Create folder
    sceIoMkdir("ux0:/data/xenity_engine", 0777);
    // Init debug file
    debugFile.open("ux0:data/xenity_engine/xenity_engine_debug.txt");
#else
    // Init debug file
    debugFile.open("xenity_engine_debug.txt");
#endif
    Print("-------- Debug initiated --------");
}