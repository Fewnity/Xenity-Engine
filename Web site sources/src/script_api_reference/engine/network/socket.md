# Socket

Add this in your code:
```cpp
#include <engine/network/network.h>
```

## Description

Class to send and received data to/from a server.<br>
Created with the NetworkManager class.<br>
The socket will be closed when the object is destroyed.

## Public methods

---
### SendData (string)
Send data as a string to the server

Parameters:
- `text`: Text to send
```cpp
void SendData(const std::string& text)
```
Code sample:
```cpp
// Fake ip
std::shared_ptr<Socket> socket = NetworkManager::CreateSocket("192.168.1.10", 6004);
if(socket)
{
    socket->SendData("Hello World!");
}
```

---
### SendData (binary)
Send binary data to the server

Parameters:
- `data`: Pointer to the data to send
- `size`: Size on byte to send
```cpp
void SendData(const char* data, int size)
```
Code sample:
```cpp
// Fake ip
std::shared_ptr<Socket> socket = NetworkManager::CreateSocket("192.168.1.10", 6004);

struct MyStruct
{
    int value0 = 1;
    int value1 = 2;
};

MyStruct myStruct = MyStruct();
if(socket)
{
    socket->SendData(reinterpret_cast<char*>(&myStruct), sizeof(MyStruct));
}
```

---
### Close
Send data as a string to the server
```cpp
void Close()
```
Code sample:
```cpp
// Fake ip
std::shared_ptr<Socket> socket = NetworkManager::CreateSocket("192.168.1.10", 6004);
if(socket)
{
    socket->SendData("Hello World!");
    socket->Close();
}
```

---
### GetIncommingData
Return recieved data since the last GetIncommingData call (updated every frame)
```cpp
std::string GetIncommingData()
```
Code sample:
```cpp
// Fake ip
std::shared_ptr<Socket> socket = NetworkManager::CreateSocket("192.168.1.10", 6004);
if(socket)
{
    std::string data = socket->GetIncommingData();
}
```