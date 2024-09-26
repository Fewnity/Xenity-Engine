// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "crash_handler.h"


#if defined(__LINUX__)
#include <csignal>
#include <execinfo.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <csignal>
#include <Windows.h>
#include <DbgHelp.h>
#endif

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/component.h>
#include <engine/assertions/assertions.h>
#include "debug.h"

void CrashHandler::Handler(int signum)
{
#if defined(_WIN32) || defined(_WIN64)
	// Create crash dump file
	std::shared_ptr<File> file = FileSystem::MakeFile("crash_dump.txt");
	bool isFileDumpOpened = file->Open(FileMode::WriteCreateFile);

	const std::string crashDetectedMessage = "!!! Crash detected !!!";

	// Print in console and file
	Debug::PrintError("\n" + crashDetectedMessage, true);
	if (isFileDumpOpened)
		file->Write(crashDetectedMessage + "\n");

	std::string errorTypeMessage;
	switch (signum)
	{
	case SIGSEGV:
		errorTypeMessage = "Segmentation fault";
		break;
	case SIGFPE:
		errorTypeMessage = "Floating point exception";
		break;
	default:
		errorTypeMessage = "Other type of exception: " + std::to_string(signum);
		break;
	}

	// Print in console and file
	Debug::PrintError(errorTypeMessage, true);
	if (isFileDumpOpened)
		file->Write(errorTypeMessage + "\n");

	const std::string stackTraceMessage = "\n------ Stack Trace ------\n";

	// Print in console and file
	Debug::PrintError(stackTraceMessage, true);
	if (isFileDumpOpened)
		file->Write(stackTraceMessage + "\n");

	SYMBOL_INFO* symbol;
	symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));

	if (symbol)
	{
		// Get process
		HANDLE process;
		process = GetCurrentProcess();
		SymInitialize(process, NULL, TRUE);

		void* stack[100];
		unsigned short frames;
		frames = CaptureStackBackTrace(0, 100, stack, NULL);
		DWORD dwDisplacement;
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		IMAGEHLP_LINE* line = (IMAGEHLP_LINE*)malloc(sizeof(IMAGEHLP_LINE));

		bool needPrintStack = false;
		int functionCount = 0;

		const std::string lineNumberMessage = "Line numbers may not be accurate";

		// Print in console and file
		Debug::Print(lineNumberMessage);
		if(isFileDumpOpened)
			file->Write(lineNumberMessage + "\n");

		for (unsigned short i = 0; i < frames; i++)
		{
			if (line)
				SymGetLineFromAddr(process, (DWORD64)(stack[frames - i - 1]), &dwDisplacement, line); // Get file infos
			SymFromAddr(process, (DWORD64)(stack[frames - i - 1]), 0, symbol); // Get symbol infos

			std::string name = symbol->Name;
			//Check functions name to print only useful functions names
			if (name == "main") // Beginning of the call stack
			{
				needPrintStack = true;
			}
			else if (name == "KiUserExceptionDispatcher") // End of the call stack
			{
				needPrintStack = false;
			}

			// Print function name and the location
			if (needPrintStack)
			{
				std::string message;
				if (line)
				{
					std::string fileName = line->FileName;
					//message = std::to_string(functionCount) + ": " + name + "() in " + fileName;
					message = std::to_string(functionCount) + ": " + name + " " + fileName + " at line " + std::to_string(line->LineNumber-1); // LineNumber is not very accurate
				}
				else 
				{
					message = std::to_string(functionCount) + ": " + name + "()";
				}

				// Print in console and file
				Debug::Print(message);
				if (isFileDumpOpened)
					file->Write(message + "\n");

				functionCount++;
			}
		}

		// Print infos about the last updated component by the GameplayManager
		std::shared_ptr<Component> lastComponent = GameplayManager::GetLastUpdatedComponent().lock();

		if (lastComponent)
		{
			std::string lastComponentMessage = "Component name: " + lastComponent->GetComponentName();
			if(lastComponent->GetGameObject())
				lastComponentMessage += "\nThis component was on the gameobject: " + lastComponent->GetGameObject()->GetName();

			// Print in console and file
			Debug::Print(lastComponentMessage);
			if (isFileDumpOpened)
				file->Write(lastComponentMessage + "\n");
		}
		else 
		{
			std::string lastComponentMessage = "The crash has occurred outside of the user game loop.";
			Debug::Print(lastComponentMessage);
			if (isFileDumpOpened)
				file->Write(lastComponentMessage + "\n");
		}

		if (isFileDumpOpened) 
		{
			file->Write("\n\n\n");
			file->Close();
		}
		free(line);
		free(symbol);
	}
	// Raise SIGBREAK signal to correctly shutdown the engine
	raise(SIGBREAK);
	exit(signum);
#elif defined(__LINUX__)
Debug::Print("AN ERROR");

const int maxStackTraceSize = 30;
	void* stackTrace[maxStackTraceSize];
	int stackTraceSize = backtrace(stackTrace, maxStackTraceSize);
	char** stackSymbols = backtrace_symbols(stackTrace, stackTraceSize);

	if (stackSymbols != nullptr) 
	{
		for (int i = 0; i < stackTraceSize; ++i) 
		{
			std::cerr << stackSymbols[i] << std::endl;
		}
		free(stackSymbols);
	}

//raise(SIGBREAK);
	exit(signum);
#endif
}

void CrashHandler::Init()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	signal(SIGSEGV, Handler); // SIGSEGV (segmentation fault)
	signal(SIGFPE, Handler);  // SIGFPE (floating point number error)
#endif
}

bool CrashHandler::CallInTry(void (*function)())
{
#if defined(_WIN32) || defined(_WIN64)
	bool errorResult = false;
	__try 
	{
		function();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{
		errorResult = true;
	}
	return errorResult;
#else
	function();
	return false;
#endif
}
