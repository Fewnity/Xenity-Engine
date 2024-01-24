#pragma once
#include <engine/api.h>

#include <string>
#include <iostream>
#include <memory>
#include <vector>

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
	*/
	static void Print(const std::string& text);

	/**
	* Print an error
	* @param text Text to print
	*/
	static void PrintError(const std::string& text);

	/**
	* Print a warning
	* @param text Text to print
	*/
	static void PrintWarning(const std::string& text);

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

private:

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

	static std::string debugText;
	static std::shared_ptr<Socket> socket;
	static float SendProfilerCooldown;
	static std::shared_ptr<File> file;
};