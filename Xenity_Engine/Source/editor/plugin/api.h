// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
* This header file is intended to be included by plugins in order for them 
* to be functional with the strict minimum.
*/

// Includes declarations of Plugin and PluginManager classes
#include "plugin_manager.h"

/**
* API_PLUGIN: Used for exporting functions from the plugin DLL 
* (i.e. CreatePlugin)
*/
#if defined(EXPORT)
	// #define API_PLUGIN __declspec(dllexport)
	#define API_PLUGIN
#elif defined(IMPORT)
	#define API_PLUGIN __declspec(dllexport)
#else
	#define API_PLUGIN
#endif

/**
* Generates the exported CreatePlugin function for the provided plugin class
*/ 
#define REGISTER_PLUGIN( pluginClass )									\
	extern "C" {														\
		API_PLUGIN Plugin* CreatePlugin() { return new pluginClass(); }	\
	}