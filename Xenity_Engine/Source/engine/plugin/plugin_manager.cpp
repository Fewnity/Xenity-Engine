#include "plugin_manager.h"

#include <xenity.h>
#include <filesystem>
#include <Windows.h>

typedef Plugin* (__cdecl* CreatePluginFunction)();

std::vector<std::unique_ptr<Plugin>> PluginManager::plugins;

//  NOTE: well I don't like that but including Windows.h in the header file
//		  creates compiling errors in input_system.h
std::vector<HINSTANCE> libs;

void PluginManager::Init()
{
	Debug::Print( "PluginManager: initializing.." );

	//  setup constants
	const std::string path = "plugins/";
	const std::string extension = ".dll";

	//  check directory existence
	//  NOTE: I think it would be better to have a static
	//        function FileSystem::GetDirectory( std::string ) 
	auto dir = std::make_shared<Directory>( path );
	if ( !dir->CheckIfExist() )
	{
		Debug::PrintWarning( "PluginManager: plugins directory not found!" );
		return;
	}

	//  find & load plugin libraries
	for ( auto& file : dir->GetAllFiles( false ) )
	{
		//  check extension
		if ( file->GetFileExtension() != extension ) return;

		const std::string path = file->GetPath();

		//  try loading library
		//  TODO: abstract library loading to either DynamicLibrary static class
		//        w/ eventually a PluginLibrary struct (which can contains the library name) 
		HINSTANCE lib = LoadLibraryA( path.c_str() );
		if ( !lib )
		{
			Debug::PrintError( "PluginManager: failed to load library '" + path + "'");
			continue;
		}

		//  instantiate plugin
		CreatePluginFunction ProcCreate = (CreatePluginFunction)GetProcAddress( lib, "CreatePlugin" );
		if ( ProcCreate )
		{
			Register( (ProcCreate)() );
		}
		else
		{
			Debug::PrintError( "PluginManager:: failed to find CreatePlugin function in library '" + path + "'" );
			
			FreeLibrary( lib );
			continue;
		}

		//  store library
		libs.push_back( lib );
		Debug::Print( "PluginManager: loaded library '" + path + "'" );
	}
}

void PluginManager::Stop()
{
	//  release plugin libraries
	for ( auto& lib : libs )
	{
		//  NOTE: it would be nice to print the target library name/path in those logs,
		//		  hence the PluginLibrary struct said earlier
		if ( FreeLibrary( lib ) )
		{
			Debug::Print( "PluginManager: released library" );
		}
		else
		{
			Debug::PrintError( "PluginManager: failed to release library!" );
		}
	}
	libs.clear();
}

void PluginManager::Register( Plugin* plugin )
{
	//auto uni_plugin = std::unique_ptr<Plugin>( plugin );
	//if ( !uni_plugin ) return;

	////  setup
	//uni_plugin->Setup();

	////  store plugin
	//plugins.push_back( uni_plugin );
}