#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <iostream>
#include <memory>

class Socket;

class API Debug
{
public:

	/**
	* Init debug system
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

	static void SendProfilerDataToServer();

	/**
	* Connect the game to an online debug console
	*/
	static void ConnectToOnlineConsole();

	/**
	* Get all the debug text into a string
	*/
	static std::string GetDebugString()
	{
		return debugText;
	}
	static float SendProfilerRate;

private:

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
};