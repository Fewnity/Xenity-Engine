#include "crash_handler.h"

#if defined(_WIN32) || defined(_WIN64)
#include <csignal>
#include <Windows.h>
#include <DbgHelp.h>
#endif

#include "debug.h"

void CrashHandler::Handler(int signum)
{
#if defined(_WIN32) || defined(_WIN64)
	Debug::PrintError("\n!!! Crash detected !!!");
	switch (signum)
	{
	case SIGSEGV:
		Debug::Print("Segmentation fault");
		break;
	case SIGFPE:
		Debug::Print("Floating point exception");
		break;
	default:
		Debug::Print("Other type of exception: " + std::to_string(signum));
		break;
	}
	Debug::Print("\n------ Stack trace ------\n");

	// Linux code
	/*const int maxStackTraceSize = 10;
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
	}*/

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
				if (line)
				{
					std::string fileName = line->FileName;
					Debug::Print(std::to_string(functionCount) + ": " + name + "() in " + fileName);
					//Debug::Print(std::to_string(functionCount) + ": " + name + " " + fileName + " at line " + std::to_string(line->LineNumber)); // LineNumber is not very accurate
				}
				else 
				{
					Debug::Print(std::to_string(functionCount) + ": " + name + "()");
				}
				functionCount++;
			}
		}
		free(line);
		free(symbol);
	}
	exit(signum); // Ou d'autres actions à effectuer pour gérer l'erreur
#endif
}

void CrashHandler::Init()
{
#if defined(_WIN32) || defined(_WIN64)
	signal(SIGSEGV, Handler); // SIGSEGV (segmentation fault)
	signal(SIGFPE, Handler);  // SIGFPE (erreurs de calcul en virgule flottante)
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
	return false;
#endif
}
