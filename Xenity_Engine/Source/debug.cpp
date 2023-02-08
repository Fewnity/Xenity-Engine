#include "debug.h"


std::ofstream Debug::debugFile;

/**
 * @brief Write text in the debug file
 */
void Debug::Print(std::string text)
{
    debugFile << text << '\n';
    debugFile.flush();
    std::cout << text << std::endl;
}

/**
 * @brief Init debug system (call once)
 *
 */
void Debug::Init()
{
    // Init debug file
    debugFile.open("xenity_engine_debug.txt");
    Print("---- Debug initiated ----");
}