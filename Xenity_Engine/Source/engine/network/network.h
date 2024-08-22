// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <vector>
#include <memory>
#if defined(__PSP__)
#include <psputility.h>
#endif

#include <engine/api.h>

/**
* @brief Class to send and received data to/from a server
*/
class API Socket
{
public:

	Socket() = delete;
	explicit Socket(int socketId)
	{
		this->m_socketId = socketId;
	}

	~Socket();

	/**
	* @brief Send data
	*/
	void SendData(const std::string& text);

	/**
	* @brief Close the socket
	*/
	void Close();

	/**
	* @brief Return recieved data during this frame
	*/
	const std::string& GetIncommingData() const
	{
		return m_incommingData;
	}

protected:
	friend class NetworkManager;

	/**
	* @brief [Internal] Read data from the socket
	*/
	void Update();

	std::string m_incommingData;
	int m_socketId = -1;
};

class API NetworkManager
{
public:
	/**
	* @brief Create a socket
	*/
	static std::shared_ptr<Socket> CreateSocket(const std::string& address, int port);


private:
	friend class Engine;
	friend class Graphics;

	/**
	* @brief [Internal] Init network manager
	*/
	static void Init();

	/**
	* @brief [Internal] Update all sockets (To call every frame)
	*/
	static void Update();

	/**
	* @brief [Internal] draw network setup menu for the PSP
	*/
	static void DrawNetworkSetupMenu();

	static bool s_needDrawMenu;
#if defined(__PSP__)
	static pspUtilityNetconfData s_pspNetworkData;
	static int s_result;
#endif
	static bool s_done;

	static std::vector< std::shared_ptr<Socket>> s_sockets;
};