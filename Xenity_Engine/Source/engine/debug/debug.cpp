#include "debug.h"

#ifdef __PSP__
#include "../../psp/debug/debug.h"
#elif __vita__
#include "../../psvita/debug/debug.h"
#include <psp2/io/stat.h>
#endif

std::ofstream Debug::debugFile;
// FILE *file;
/**
 * @brief Write text in the debug file
 */
void Debug::Print(std::string text)
{
    text += '\n';
#ifdef __PSP__
    // PspDebugPrint(text);
#elif __vita__
    // PsVitaDebugPrint(text);
    debugFile << text;
    debugFile.flush();
#else
    // debugFile << text << '\n';
    debugFile << text;
    debugFile.flush();
    // std::cout << text << std::endl;
    std::cout << text;
#endif
}

/**
 * @brief Init debug system (call once)
 *
 */
void Debug::Init()
{
    // debugFile = std::ofstream();
#ifdef __PSP__
    // PspDebugInit();
    // file = fopen("xenity_engine_debug.txt", "a");
    // fprintf(file, "uwu\n");
    // fclose(fp);
    // file.write("uwu");
    //  debugFile.open("ms0:/xenity_engine_debug.txt");
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