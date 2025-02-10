#include "update_checker.h"

#include <curl/curl.h>
#include <json.hpp>

#include <engine/network/network.h>  
#include <engine/debug/debug.h>
#include <engine/constants.h>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void UpdateChecker::CheckForUpdate(Event<bool>* onUpdateCheckedEvent)
{
	XASSERT(onUpdateCheckedEvent, "onUpdateCheckedEvent is nullptr");

	bool updateFound = false;
	CURL* curl = curl_easy_init();
	if (curl)
	{
		CURLcode res;
		std::string readBuffer;

		//curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/Fewnity/Xenity-Engine-SDK/releases");
		/*curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/skiff/PS3-Toolbox/releases");*/
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/ocornut/imgui/releases");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Xenity");
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "application/vnd.github+json");
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
					const std::string tagName = release["tag_name"];
					int major = 0;
					int minor = 0;
					int patch = 0;
#if defined(_WIN32) || defined(_WIN64)
					sscanf_s(tagName.c_str(), "v%d.%d.%d\n", &major, &minor, &patch);
#elif defined(__LINUX__)
					sscanf(tagName.c_str(), "v%d.%d.%d\n", &major, &minor, &patch);
#endif
					Debug::Print("Tag name: " + tagName);
					if (major > ENGINE_MAJOR_VERSION)
					{
						updateFound = true;
						break;
					}
					else if (major < ENGINE_MAJOR_VERSION)
					{
						continue;
					}

					if (minor > ENGINE_MINOR_VERSION)
					{
						updateFound = true;
						break;
					}
					else if (minor < ENGINE_MINOR_VERSION)
					{
						continue;
					}

					if (patch > ENGINE_PATCH_VERSION)
					{
						updateFound = true;
						break;
					}
					else if (patch < ENGINE_PATCH_VERSION)
					{
						continue;
					}
				}
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[UpdateChecker::CheckForUpdate] failed to parse json", true);
			}
		}
		else 
		{
			Debug::PrintError("[UpdateChecker::CheckForUpdate] failed to check update", true);
		}
	}
	onUpdateCheckedEvent->Trigger(updateFound);
}
