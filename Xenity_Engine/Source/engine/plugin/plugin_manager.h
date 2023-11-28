#pragma once

#include "plugin.h"

#include <string>
#include <vector>
#include <memory>

class PluginManager
{
public:
	template <typename T>
	static void Register( const std::string name )
	{
		static_assert( 
			std::is_base_of<Plugin, T>::value, 
			"Called PluginManager::Register with a non-Plugin type!" 
		);

		auto plugin = std::make_unique<T>();
		plugin->Setup();

		plugins.push_back( plugin );
	}

private:
	static std::vector<std::unique_ptr<Plugin>> plugins;
};