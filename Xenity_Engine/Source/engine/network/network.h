#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <vector>

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

    /**
    * Send data
    */
    void SendData(std::string text);

    /**
    * Read data from the socket
    */
    void Update();

    /**
    * Return recieved data during this frame
    */
    std::string GetIncommingData() 
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
    * [Internal] Create a socket
    */
    static Socket *CreateSocket(std::string address, int port);

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
    static std::vector<Socket *> sockets;
};