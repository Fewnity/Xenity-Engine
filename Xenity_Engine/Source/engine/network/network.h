#pragma once
#include <engine/api.h>

#include <string>
#include <vector>
#include <memory>

#if defined(__PSP__)
#include <psputility.h>
#endif

class API Socket
{
public:

	Socket() = delete;
	Socket(int socketId)
	{
		this->socketId = socketId;
	}

	~Socket();

	/**
	* Send data
	*/
	void SendData(const std::string& text);

	/**
	* Read data from the socket
	*/
	void Update();

	void Close();

	/**
	* Return recieved data during this frame
	*/
	std::string GetIncommingData() const
	{
		return incommingData;
	}

private:
	std::string incommingData;
	int socketId = -1;
};

class API NetworkManager
{
public:

	/**
	* [Internal] Init network manager
	*/
	static void Init();

	/**
	* [Internal] Update all sockets (To call every frame)
	*/
	static void Update();

	/**
	* Create a socket
	*/
	static std::shared_ptr<Socket> CreateSocket(const std::string& address, int port);

	/**
	* [Internal] draw network setup menu for the PSP
	*/
	static void DrawNetworkSetupMenu();

	static bool needDrawMenu;
#if defined(__PSP__)
	static pspUtilityNetconfData pspNetworkData;
	static int result;
#endif
	static bool done;

private:
	static std::vector< std::shared_ptr<Socket>> sockets;
};