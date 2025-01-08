#include "update_checker.h"

#include <engine/network/network.h>  
#include <engine/debug/debug.h>

bool UpdateChecker::CheckForUpdate()
{
	std::shared_ptr<Socket> socket = NetworkManager::CreateSocket("www.api.github.com", 443);
	socket->SendData("GET / HTTP/1.1\r\n");
	socket->SendData("Host: www.api.github.com\r\nAccept: text/html\r\n\r\n");
	std::string data;
	while (data.empty())
	{
	NetworkManager::Update();
	data = socket->GetIncommingData();

	}
	Debug::Print(data);

    return false;
}
