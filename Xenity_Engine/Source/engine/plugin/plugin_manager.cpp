#include "plugin_manager.h"

#include <xenity.h>
#include <filesystem>
#include <Windows.h>

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

		//  try loading library
		//  TODO: abstract library loading to either DynamicLibrary static class
		//        w/ eventually a PluginLibrary struct (which can contains the library name) 
		HINSTANCE lib = LoadLibraryA( file->GetPath().c_str() );
		if ( !lib )
		{
			Debug::PrintError( "PluginManager: failed to load library '" + file->GetPath() + "'");
			continue;
		}
		Debug::Print( "PluginManager: loaded library '" + file->GetPath() + "'" );

		//  register
		libs.push_back( lib );
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
