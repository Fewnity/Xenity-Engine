// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <string>
#include <vector>
#include <memory>

#if defined(__PSP__)
#include <psputility.h>
#endif

/**
* @brief Class to send and received data to/from a server
*/
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
	* @brief Send data
	*/
	void SendData(const std::string& text);

	/**
	* @brief [Internal] Read data from the socket
	*/
	void Update();

	/**
	* @brief Close the socket
	*/
	void Close();

	/**
	* @brief Return recieved data during this frame
	*/
	const std::string& GetIncommingData() const
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
	* @brief [Internal] Init network manager
	*/
	static void Init();

	/**
	* @brief [Internal] Update all sockets (To call every frame)
	*/
	static void Update();

	/**
	* @brief Create a socket
	*/
	static std::shared_ptr<Socket> CreateSocket(const std::string& address, int port);

	/**
	* @brief [Internal] draw network setup menu for the PSP
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