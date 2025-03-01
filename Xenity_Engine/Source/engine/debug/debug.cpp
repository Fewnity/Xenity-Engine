// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "debug.h"

#include <iostream>
#if defined(__PSP__)
#include <pspkernel.h>
#include <psp/debug/debug.h>
#elif defined(__vita__)
#include <psvita/debug/debug.h>
#include <psp2/io/stat.h>
#endif

#include <engine/engine_settings.h>
#include <engine/network/network.h>
#include <engine/engine.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/time/time.h>
#include <engine/audio/audio_manager.h>
#include <engine/constants.h>
#include "performance.h"

std::shared_ptr<File> Debug::s_file = nullptr;
std::string Debug::s_debugText = "";
std::shared_ptr<Socket> Debug::s_socket = nullptr;

float Debug::s_sendProfilerCooldown = 0;
float Debug::s_sendProfilerDelay = 0.2f;
std::vector<DebugHistory> Debug::s_debugMessageHistory;

Event<> Debug::s_onDebugLogEvent;
size_t Debug::s_lastDebugMessageHistoryIndex = -1;
MyMutex* debugMutex = nullptr;

/**
 * Print an error in the console and the debug file
 */
void Debug::PrintError(const std::string& text, bool hideInEditorConsole)
{
	const std::string finalText = text + '\n';
	const std::string textWithoutColor = "[ERROR] " + finalText;
	const std::string textWithColor = "\033[31m" + textWithoutColor + "\033[37m";
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	PrintInConsole(textWithColor);
#else
	PrintInConsole(textWithoutColor); // Do not print in color on game consoles
#endif
	if (!EngineSettings::values.useDebugger || debugMutex == nullptr || !Engine::IsRunning(false))
	{
		return;
	}

	debugMutex->Lock();
	if(!hideInEditorConsole)
		AddMessageInHistory(text, DebugType::Error);
	PrintInOnlineConsole(text);
	PrintInFile(textWithoutColor);
	if (!hideInEditorConsole)
		s_debugText += textWithoutColor;
	s_onDebugLogEvent.Trigger();
	debugMutex->Unlock();
}

/**
 * Print a warning in the console and the debug file
 */
void Debug::PrintWarning(const std::string& text, bool hideInEditorConsole)
{
	const std::string finalText = text + '\n';
	const std::string textWithoutColor = "[WARNING] " + finalText;
	const std::string textWithColor = "\033[33m" + textWithoutColor + "\033[37m";
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	PrintInConsole(textWithColor);
#else
	PrintInConsole(textWithoutColor); // Do not print in color on game consoles
#endif
	if (!EngineSettings::values.useDebugger || debugMutex == nullptr || !Engine::IsRunning(false))
	{
		return;
	}

	debugMutex->Lock();
	if (!hideInEditorConsole)
		AddMessageInHistory(text, DebugType::Warning);
	PrintInOnlineConsole(text);
	PrintInFile(textWithoutColor);
	if (!hideInEditorConsole)
		s_debugText += textWithoutColor;
	s_onDebugLogEvent.Trigger();
	debugMutex->Unlock();
}

/**
 * @brief Print text in the console and the debug file
 */
void Debug::Print(const std::string& text, bool hideInEditorConsole)
{
	const std::string finalText = text + '\n';
	const std::string textWithColor = "\033[37m" + finalText;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	PrintInConsole(textWithColor);
#else
	PrintInConsole(finalText); // Do not print in color on game consoles
#endif
	if (!EngineSettings::values.useDebugger || debugMutex == nullptr || !Engine::IsRunning(false))
	{
		return;
	}

	debugMutex->Lock();
	if (!hideInEditorConsole)
		AddMessageInHistory(text, DebugType::Log);
	PrintInOnlineConsole(text);

	PrintInFile(finalText);
	if (!hideInEditorConsole)
		s_debugText += finalText; // Disable because cause crashes, why? Maybe thread?
	s_onDebugLogEvent.Trigger();
	debugMutex->Unlock();
}

void Debug::SendProfilerDataToServer()
{
	if (s_socket)
	{
		if (s_sendProfilerCooldown <= 0)
		{
			s_sendProfilerCooldown = s_sendProfilerDelay;
			if (EngineSettings::values.useProfiler)
			{
				//Add profiler texts
				for (const auto& kv : Performance::s_profilerCategories)
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
			s_sendProfilerCooldown -= Time::GetUnscaledDeltaTime();
		}
	}
}

void Debug::PrintInConsole(const std::string& text)
{
#if defined(__PSP__)
	printf(text.c_str());
	// PspDebugPrint(text);
#elif defined(__vita__)
	// PsVitaDebugPrint(text);
#elif defined(_EE)
	printf(text.c_str());
#elif defined(__PS3__)
	std::cout << text;
#else
	std::cout << text;
#endif
}

void Debug::PrintInFile(const std::string& text)
{
	if (s_file) 
	{
#if defined(__PSP__) // On psp there is a problem with files, so we need to close and reopen the file
		s_file->Open(FileMode::WriteOnly);
		s_file->Write(text);
		s_file->Close();
#else
		s_file->Write(text);
#endif
	}
}

void Debug::ClearDebugLogs()
{
	if (debugMutex == nullptr)
		return;

	debugMutex->Lock();
	s_debugMessageHistory.clear();
	s_lastDebugMessageHistoryIndex = -1;
	s_debugText.clear();
	debugMutex->Unlock();
}

void Debug::AddMessageInHistory(const std::string& message, DebugType messageType)
{
#if defined(EDITOR)
	if (!Engine::IsRunning(false))
		return;

	const size_t historySize = s_debugMessageHistory.size();
	bool found = false;
	for (size_t i = 0; i < historySize; i++)
	{
		DebugHistory& historyItem = s_debugMessageHistory[i];
		if (historyItem.type == messageType && historyItem.message == message)
		{
			historyItem.count++;
			found = true;
			s_lastDebugMessageHistoryIndex = i;
			break;
		}
	}

	if (!found)
	{
		DebugHistory historyItem;
		historyItem.message = message;
		historyItem.count = 1;
		historyItem.type = messageType;
		s_debugMessageHistory.push_back(historyItem);
		s_lastDebugMessageHistoryIndex = s_debugMessageHistory.size() - 1;
	}
#endif
}

void Debug::PrintInOnlineConsole(const std::string& text)
{
	if (s_socket)
	{
		const std::string finalText = "{1;" + text + "}";
		s_socket->SendData(finalText);
	}
}

void Debug::ConnectToOnlineConsole()
{
	Print("Connecting to online console...", true);
	s_socket = NetworkManager::CreateSocket("192.168.1.136", 6004);
}

/**
 * @brief Init debug system (call once)
 *
 */
int Debug::Init()
{
	if (!EngineSettings::values.useDebugger)
		return 0;

	debugMutex = new MyMutex("DebugMutex");

	std::string fileName = DEBUG_LOG_FILE;
#if defined(__vita__)
	fileName = PSVITA_DEBUG_LOG_FOLDER + fileName;
#endif
	FileSystem::s_fileSystem->Delete(fileName);

	s_file = FileSystem::MakeFile(fileName);

	if (!s_file->Open(FileMode::WriteCreateFile))
	{
		s_file = nullptr;
		PrintError("-------- Debug file not created --------");
		return -1;
	}

	Print("-------- Debug initiated --------", true);
	return 0;
}