#include "update_checker.h"

#include <engine/network/network.h>  
#include <engine/debug/debug.h>
#include <curl/curl.h>

#include <json.hpp>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool UpdateChecker::CheckForUpdate()
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		CURLcode res;
		std::string readBuffer;

		//curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/Fewnity/Xenity-Engine-SDK/releases");
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/skiff/PS3-Toolbox/releases");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Xenity");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res == CURLE_OK)
		{
			try
			{
				const json j = json::parse(readBuffer);
				for (auto& release : j)
				{
					std::string tagName = release["tag_name"];
					Debug::Print("Tag name: " + tagName);
					/*if (tagName == "v1.0.0")
					{
						return true;
					}*/
				}
			}
			catch (const std::exception&)
			{

			}
		}
		else 
		{
			return false;
		}
	}

	return false;
}
