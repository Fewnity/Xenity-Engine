#pragma once
#include <engine/api.h>

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <engine/event_system/event_system.h>

class Socket;
class File;

enum class DebugType {
	Log,
	Warning,
	Error,
};

class DebugHistory 
{
public:
	DebugType type = DebugType::Log;
	int count = 0;
	std::string message;
};

/**
 * Used to print text in a console/file or remotely to a server
 */
class API Debug
{
public:

	/**
	* [Internal] Init debug system
	*/
	static int Init();

	/**
	* Print a text
	* @param text Text to print
	* @param hideInConsole If true, the text will not be printed in the console
	*/
	static void Print(const std::string& text, bool hideInConsole = false);

	/**
	* Print an error
	* @param text Text to print
	* @param hideInConsole If true, the text will not be printed in the console
	*/
	static void PrintError(const std::string& text, bool hideInConsole = false);

	/**
	* Print a warning
	* @param text Text to print
	* @param hideInConsole If true, the text will not be printed in the console
	*/
	static void PrintWarning(const std::string& text, bool hideInConsole = false);

	/**
	 * [Internal] Send all profiler data to the debug server
	 */
	static void SendProfilerDataToServer();

	/**
	* [Internal] Connect the game to an online debug console
	*/
	static void ConnectToOnlineConsole();

	/**
	* [Internal] Get all the debug text into a string
	*/
	static std::string GetDebugString()
	{
		return debugText;
	}

	/**
	* [Internal] Clear all logs (the file is untouched)
	*/
	static void ClearDebugLogs();


	// [Internal] Lower is higher speed
	static float SendProfilerDelay;
	static std::vector<DebugHistory> debugMessageHistory;

	/**
	* Get the event when a debug message is printed
	*/
	static Event<>& GetOnDebugLogEvent()
	{
		return OnDebugLogEvent;
	}

private:

	/**
	* Add a message in the history
	* @param message Message to add
	* @param messageType Type of the message
	*/
	static void AddMessageInHistory(const std::string& message, DebugType messageType);

	/**
	* Send text via the socket to the online debug console
	* @param text Text to send
	*/
	static void PrintInOnlineConsole(const std::string& text);

	/**
	* Print text to the cmd
	* @param text Text to print
	*/
	static void PrintInConsole(const std::string& text);

	/**
	* Write text to the debug file
	* @param text Text to write
	*/
	static void PrintInFile(const std::string& text);

	static Event<> OnDebugLogEvent;
	static std::string debugText;
	static std::shared_ptr<Socket> socket;
	static float SendProfilerCooldown;
	static std::shared_ptr<File> file;
};