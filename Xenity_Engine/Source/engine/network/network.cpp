#include "network.h"

#include "../debug/debug.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstring>

#if defined(__vita__)
#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#endif

#if defined(__PSP__)
#include <pspnet.h>
#include <pspnet_apctl.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <psputility.h>
#include <psputils.h>
#include "../../psp/gu2gl.h"
pspUtilityNetconfData NetworkManager::pspNetworkData;
struct pspUtilityNetconfAdhoc adhocparam;
bool NetworkManager::done = false;
int NetworkManager::result = -1;
#endif

std::vector<Socket *> NetworkManager::sockets;
bool NetworkManager::needDrawMenu = true;

void NetworkManager::Init()
{
#if defined(__PSP__)
    sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityLoadNetModule(PSP_NET_MODULE_INET);

    sceNetInit(128 * 1024, 42, 4 * 1024, 42, 4 * 1024);
    sceNetInetInit();
    sceNetApctlInit(0x8000, 48);

    memset(&pspNetworkData, 0, sizeof(pspNetworkData));
    pspNetworkData.base.size = sizeof(pspNetworkData);
    pspNetworkData.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
    pspNetworkData.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
    pspNetworkData.base.graphicsThread = 17;
    pspNetworkData.base.accessThread = 19;
    pspNetworkData.base.fontThread = 18;
    pspNetworkData.base.soundThread = 16;
    pspNetworkData.action = PSP_NETCONF_ACTION_CONNECTAP;

    memset(&adhocparam, 0, sizeof(adhocparam));
    pspNetworkData.adhocparam = &adhocparam;

    sceUtilityNetconfInitStart(&pspNetworkData);
#elif defined(__vita__)
    Debug::ConnectToOnlineConsole();
#endif
}

void NetworkManager::Update()
{
    int socketCount = sockets.size();
    for (int i = 0; i < socketCount; i++)
    {
        sockets[i]->Update();
    }
}

void Socket::Update()
{
    if (socketId < 0)
        return;

    char recvBuff[1024];
    int recvd_len;

    incommingData = "";

    // Read a maximum of 1022 char in one loop
    while ((recvd_len = recv(socketId, recvBuff, 1023, 0)) > 0) // if recv returns 0, the socket has been closed. (Sometimes yes, sometimes not, lol)
    {
        recvBuff[recvd_len] = 0;
        incommingData += recvBuff;
    }
}

void Socket::SendData(std::string text)
{
    if (socketId < 0)
        return;

    int InfoLentgh = text.size();
    send(socketId, text.c_str(), InfoLentgh, 0); // Send data to server
}

void NetworkManager::DrawNetworkSetupMenu()
{
    if (!done)
    {
#if defined(__PSP__)
        sceGuFinish();
        sceGuSync(0, 0);

        switch (sceUtilityNetconfGetStatus())
        {
        case PSP_UTILITY_DIALOG_NONE:
        {
            result = pspNetworkData.base.result;
            Debug::Print("Network setup: " + std::to_string(result));
            if (result == 0)
            {
                Debug::ConnectToOnlineConsole();
            }
            done = true;
        }
        break;

        case PSP_UTILITY_DIALOG_VISIBLE:
            sceUtilityNetconfUpdate(1);
            break;

        case PSP_UTILITY_DIALOG_QUIT:
            sceUtilityNetconfShutdownStart();
            break;

        case PSP_UTILITY_DIALOG_FINISHED:
            break;

        default:
            break;
        }
#endif
    }
}

Socket *NetworkManager::CreateSocket(std::string address, int port)
{
    // return nullptr;
    int newSocketId = 1;

    struct sockaddr_in serv_addr;

    // memset(recvBuff, '0', sizeof(recvBuff));

    if ((newSocketId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Debug::Print("\n Error : Could not create socket");
        return nullptr;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr) <= 0)
    {
        Debug::Print("\n inet_pton error occured");
        return nullptr;
    }
    if (connect(newSocketId, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        Debug::Print("\n Error : Connect Failed");
        return nullptr;
    }
    int i = 1;
    if (setsockopt(newSocketId, SOL_SOCKET, SO_NONBLOCK, (char *)&i, sizeof(i)) < 0)
    {
        Debug::Print("Failed to change socket flags");
        return nullptr;
    }

    Socket *myNewSocket = new Socket(newSocketId);
    return myNewSocket;
}