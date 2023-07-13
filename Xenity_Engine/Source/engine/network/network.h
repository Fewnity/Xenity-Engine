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
#include <pspnet.h>
#include <pspnet_apctl.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <psputility.h>
#include <psputils.h>
#endif

class API Socket
{
public:
    Socket() = delete;
    Socket(int socketId)
    {
        this->socketId = socketId;
    }
    void SendData(std::string text);
    void Update();
    std::string incommingData;

private:
    int socketId = -1;
};

class API NetworkManager
{
public:
    static void Init();

    // static void ConnectToConsole();
    static void Update();
    static Socket *CreateSocket(std::string address, int port);
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