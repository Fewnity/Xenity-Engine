# NetworkManager

Add this in your code:
```cpp
#include <engine/network/network.h>
```

## Description

Class to create sockets and to manage networking settings.

## Static methods

---
### CreateSocket
Create a socket.<br>
Returns nullptr if the socket creation has failed.

Parameters:
- `address`: IP address
- `port`: Port number
```cpp
std::shared_ptr<Socket> CreateSocket(const std::string& address, int port)
```
Code sample:
```cpp
// Fake ip
std::shared_ptr<Socket> socket = NetworkManager::CreateSocket("192.168.1.10", 6004);
// Check if the socket has been created
if(socket)
{
    // ...
}
```

---
### CreateSocket
Show the network setup menu for the PSP.<br>
This function will only work on the PSP platform.

To make networking working on PSP, the user has to manually connect to a wifi.<br>
For this, a menu of the PSP OS ask to the user to setup a network connection. 
```cpp
void ShowPSPNetworkSetupMenu()
```
Code sample:
```cpp
NetworkManager::ShowPSPNetworkSetupMenu();
```