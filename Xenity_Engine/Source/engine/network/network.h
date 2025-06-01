// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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
		m_socketId = socketId;
	}
	Socket(const Socket& other) = delete;
	Socket& operator=(const Socket&) = delete;

	~Socket();

	/**
	* @brief Send data as a string to the server
	*/
	void SendData(const std::string& text);

	/**
	* @brief Send binary data to the server
	*/
	void SendData(const char* data, int size);

	/**
	* @brief Close the socket
	*/
	void Close();

	/**
	* @brief Return recieved data during this frame
	*/
	[[nodiscard]] std::string GetIncommingData()
	{
		const std::string data = m_incommingData;
		m_incommingData.clear();
		return data;
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

/**
* @brief Class to create sockets and to manage networking settings
*/
class API NetworkManager
{
public:
	/**
	* @brief Create a socket
	* @bried Returns nullptr if the socket creation has failed
	*/
	[[nodiscard]] static std::shared_ptr<Socket> CreateSocket(const std::string& address, int port);

	/**
	* @brief Show the network setup menu for the PSP
	* @brief This function will only work on the PSP platform.
	*/
	static void ShowPSPNetworkSetupMenu();

private:
	friend class Engine;
	friend class Graphics;
	friend class UpdateChecker;

	[[nodiscard]] static std::shared_ptr<Socket> GetClientSocket();

	/**
	* @brief [Internal] Init network manager
	*/
	static void Init();

	static void Stop();

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