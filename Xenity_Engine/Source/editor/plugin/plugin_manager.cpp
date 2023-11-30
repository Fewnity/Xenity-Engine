#include "plugin_manager.h"

#include <xenity.h>
#include <filesystem>
#include <Windows.h>
#include <editor/compiler.h>

typedef Plugin* (__cdecl* CreatePluginFunction)();

std::vector<std::unique_ptr<Plugin>> PluginManager::plugins;

//  NOTE: well I don't like that but including Windows.h in the header file
//		  creates compiling errors in input_system.h
std::vector<HINSTANCE> libs;

void PluginManager::Init()
{
	Debug::Print("[PluginManager::Init] Initalizing..");
	
#if defined(EDITOR)
	Debug::Print("[PluginManager::Init] Compiling..");
	Compiler::CompilePlugin( 
		Platform::P_Windows, 
		"plugins\\test\\" 
	);
#endif

	// Setup constants
	const std::string path = "plugins\\";
	const std::string extension = ".dll";

	// Check directory existence
	// NOTE: I think it would be better to have a static
	//       function FileSystem::GetDirectory( std::string ) 
	auto dir = std::make_shared<Directory>(path);
	if (!dir->CheckIfExist())
	{
		Debug::PrintWarning("[PluginManager::Init] Plugins directory not found!");
		return;
	}

	// Load plugin libraries
	for (auto& file : dir->GetAllFiles(false))
	{
		// Check extension
		if (file->GetFileExtension() != extension) return;

		const std::string path = file->GetPath();

		// Try loading DLL
		//  TODO: abstract library loading to either DynamicLibrary static class
		//        w/ eventually a PluginLibrary struct (which can contains the library name) 
		HINSTANCE lib = LoadLibraryA(path.c_str());
		if ( !lib )
		{
			auto error = "WindowsError(" + std::to_string(GetLastError()) + ")";
			Debug::PrintError("PluginManager: failed to load library '" + path + "': " + error);
			continue;
		}

		//  instantiate plugin
		CreatePluginFunction ProcCreate = (CreatePluginFunction)GetProcAddress(lib, "CreatePlugin");
		if (!ProcCreate)
		{
			Debug::PrintError("[PluginManager::Init] Failed to find CreatePlugin function in DLL '" + path + "'");
			
			FreeLibrary(lib);
			continue;
		}

		// Store DLL
		libs.push_back(lib);
		Debug::Print("[PluginManager::Init] Loaded DLL '" + path + "'");

		// Register plugin
		Register((ProcCreate)());
	}
}

void PluginManager::Stop()
{
	// Release plugins
	plugins.clear();

	// Release DLLs
	for (auto& lib : libs)
	{
		//  NOTE: it would be nice to print the target library name/path in those logs,
		//		  hence the PluginLibrary struct said earlier
		if (FreeLibrary(lib))
		{
			Debug::Print("[PluginManager::Stop] Released a plugin DLL");
		}
		else
		{
			Debug::PrintError("[PluginManager::Stop] Failed to release a plugin DLL!");
		}
	}
	libs.clear();
}

void PluginManager::Register(Plugin* plugin)
{
	printf("Plugin Adress: %p\n", plugin);
	if (!plugin) return;

	// Setup
	plugin->Setup();

	auto& infos = plugin->GetInfos();
	printf(
		"- Name: %s\n- Version: %s\n- Description: %s\n- Author: %s\n", 
		infos.name.c_str(), 
		infos.version.c_str(), 
		infos.description.c_str(), 
		infos.author.c_str()
	);

	// Store into plugins
	plugins.emplace_back(plugin);
	Debug::Print("[PluginManager::Register] Registered plugin '" + infos.name + "'");
}