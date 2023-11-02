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
#include "performance.h"
#include "../../xenity.h"

std::shared_ptr<File> file = nullptr;
std::string Debug::debugText = "";
std::shared_ptr<Socket> Debug::socket = nullptr;

float Debug::SendProfilerCooldown = 0;
float Debug::SendProfilerRate = 0.2f;

/**
 * Print an error in the console and the debug file
 */
void Debug::PrintError(const std::string& text)
{
	if (!EngineSettings::useDebugger)
		return;

	PrintInOnlineConsole(text);
	std::string finalText = text + '\n';
	std::string textWithoutColor = "[ERROR] " + finalText;
	std::string textWithColor = "\033[31m" + textWithoutColor;
	PrintInConsole(textWithColor);
	PrintInFile(textWithoutColor);
	debugText += textWithoutColor;
}

/**
 * Print a warning in the console and the debug file
 */
void Debug::PrintWarning(const std::string& text)
{
	if (!EngineSettings::useDebugger)
		return;

	PrintInOnlineConsole(text);
	std::string finalText = text + '\n';
	std::string textWithoutColor = "[WARNING] " + finalText;
	std::string textWithColor = "\033[33m" + textWithoutColor;
	PrintInConsole(textWithColor);
	PrintInFile(textWithoutColor);
	debugText += textWithoutColor;
}

void Debug::SendProfilerDataToServer()
{
	if (socket)
	{
		if (SendProfilerCooldown <= 0)
		{
			SendProfilerCooldown = SendProfilerRate;
			if (EngineSettings::useProfiler)
			{
				//Add profiler texts
				for (const auto& kv : Performance::profilerCategories)
				{
					std::string dat = "--- " + kv.first;
					if (kv.second->profilerList.count(kv.first) != 0)
					{
						dat += ": " + std::to_string(kv.second->profilerList[kv.first]->GetValue()) + ", avg " + std::to_string(kv.second->profilerList[kv.first]->average);
					}
					for (const auto& kv2 : kv.second->profilerList)
					{
						dat += "\n" + kv2.first + " " + std::to_string(kv2.second->average) + " " + std::to_string(kv2.second->GetValue());
					}
					PrintInOnlineConsole(dat);
				}
				PrintInOnlineConsole("");
			}
		}
		else 
		{
			SendProfilerCooldown -= Time::GetUnscaledDeltaTime();
		}
	}
}

void Debug::PrintInConsole(const std::string& text)
{
#if defined(__PSP__)
	// PspDebugPrint(text);
#elif defined(__vita__)
	// PsVitaDebugPrint(text);
#else
	std::cout << text;
#endif
}

void Debug::PrintInFile(const std::string& text)
{
	if (file && file.use_count() == 2)
		file->Write(text);
}

/**
 * @brief Print text in the console and the debug file
 */
void Debug::Print(const std::string& text)
{
	if (!EngineSettings::useDebugger)
		return;

	PrintInOnlineConsole(text);
	std::string finalText = text + '\n';
	std::string newString = "\033[37m" + finalText;
	PrintInConsole(newString);
	PrintInFile(finalText);
	//debugText += finalText; // Disable because cause crashes, why? Maybe thread?
}

void Debug::PrintInOnlineConsole(const std::string& text)
{
	if (socket)
	{
		std::string finalText = "{1;" + text + "}";
		socket->SendData(finalText);
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
int Debug::Init()
{
	if (!EngineSettings::useDebugger)
		return 0;

	std::string fileName = "xenity_engine_debug.txt";
#if defined(__vita__)
	fileName = "data\\xenity_engine\\" + fileName;
#endif
	FileSystem::fileSystem->DeleteFile(fileName);

	file = FileSystem::MakeFile(fileName);
	file->Open(true);

	if (!file->CheckIfExist())
	{
		Print("-------- Debug file not created --------");
		return -1;
	}

	Print("-------- Debug initiated --------");
	return 0;
}