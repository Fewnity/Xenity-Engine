#pragma once

/**
 * [Internal]
 */

#include "plugin.h"

#include <string>
#include <vector>
#include <memory>

class PluginManager
{
public:
	static void Init();
	static void Stop();

	/*template <typename T>
	static void Register()
	{
		static_assert( 
			std::is_base_of<Plugin, T>::value, 
			"Called PluginManager::Register with a non-Plugin type!" 
		);

		Register( new T() );
	}*/
	static void Register( Plugin* plugin );

private:
	static std::vector<std::unique_ptr<Plugin>> plugins;
};