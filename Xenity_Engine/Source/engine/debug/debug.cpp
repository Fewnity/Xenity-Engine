// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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
	if (!EngineSettings::values.useDebugger || debugMutex == nullptr || !Engine::IsRunning(false))
		return;

	debugMutex->Lock();
	if(!hideInEditorConsole)
		AddMessageInHistory(text, DebugType::Error);
	PrintInOnlineConsole(text);
	const std::string finalText = text + '\n';
	const std::string textWithoutColor = "[ERROR] " + finalText;
	const std::string textWithColor = "\033[31m" + textWithoutColor;
	PrintInConsole(textWithColor);
	PrintInFile(textWithoutColor);
	if (!hideInEditorConsole)
		s_debugText += textWithoutColor;
	debugMutex->Unlock();
	s_onDebugLogEvent.Trigger();
}

/**
 * Print a warning in the console and the debug file
 */
void Debug::PrintWarning(const std::string& text, bool hideInEditorConsole)
{
	if (!EngineSettings::values.useDebugger || debugMutex == nullptr || !Engine::IsRunning(false))
		return;

	debugMutex->Lock();
	if (!hideInEditorConsole)
		AddMessageInHistory(text, DebugType::Warning);
	PrintInOnlineConsole(text);
	const std::string finalText = text + '\n';
	const std::string textWithoutColor = "[WARNING] " + finalText;
	const std::string textWithColor = "\033[33m" + textWithoutColor;
	PrintInConsole(textWithColor);
	PrintInFile(textWithoutColor);
	if (!hideInEditorConsole)
		s_debugText += textWithoutColor;
	debugMutex->Unlock();
	s_onDebugLogEvent.Trigger();
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
#else
	std::cout << text;
#endif
}

void Debug::PrintInFile(const std::string& text)
{
	if (s_file)
		s_file->Write(text);
}

/**
 * @brief Print text in the console and the debug file
 */
void Debug::Print(const std::string& text, bool hideInEditorConsole)
{
	if (!EngineSettings::values.useDebugger || debugMutex == nullptr || !Engine::IsRunning(false))
		return;

	debugMutex->Lock();
	if (!hideInEditorConsole)
		AddMessageInHistory(text, DebugType::Log);
	PrintInOnlineConsole(text);
	const std::string finalText = text + '\n';
	const std::string newString = "\033[37m" + finalText;
	PrintInConsole(newString);
	PrintInFile(finalText);
	if (!hideInEditorConsole)
		s_debugText += finalText; // Disable because cause crashes, why? Maybe thread?
	debugMutex->Unlock();
	s_onDebugLogEvent.Trigger();
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
	Print("Connect to online console...", true);
	s_socket = NetworkManager::CreateSocket("", 6004);
}

/**
 * @brief Init debug system (call once)
 *
 */
int Debug::Init()
{
	if (!EngineSettings::values.useDebugger)
		return 0;

	std::string fileName = "xenity_engine_debug.txt";
#if defined(__vita__)
	fileName = "ux0:data/xenity_engine/" + fileName;
#endif
	FileSystem::s_fileSystem->Delete(fileName);

	s_file = FileSystem::MakeFile(fileName);

	if (!s_file->Open(FileMode::WriteCreateFile))
	{
		Print("-------- Debug file not created --------", true);
		return -1;
	}
	debugMutex = new MyMutex("DebugMutex");

	Print("-------- Debug initiated --------", true);
	return 0;
}