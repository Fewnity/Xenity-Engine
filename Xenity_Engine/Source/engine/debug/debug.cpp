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
 * @brief Write text in the debug file
 */
void Debug::Print(std::string text)
{
    text += '\n';
#ifdef __PSP__
    // PspDebugPrint(text);
    file->Write(text);
#elif __vita__
    // PsVitaDebugPrint(text);
    debugFile << text;
    debugFile.flush();
#else
    debugFile << text;
    debugFile.flush();
    std::cout << text;
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
    FileSystem::fileSystem->DeleteFile("xenity_engine_debug.txt");
    file = new File("xenity_engine_debug.txt");
#elif __vita__
    // PsVitaDebugInit();
    sceIoMkdir("ux0:/data/xenity_engine", 0777);
    debugFile.open("ux0:data/xenity_engine/xenity_engine_debug.txt");
#else
    debugFile.open("xenity_engine_debug.txt");
#endif
    // Init debug file
    // debugFile.open("xenity_engine_debug.txt");
    Print("-------- Debug initiated --------");
}