#include "compiler.h"
#include "../engine/asset_management/project_manager.h"
#include "../engine/debug/debug.h"
#include <thread>
#include "../engine/dynamic_lib/dynamic_lib.h"
#include "../engine/engine.h"
#include "../engine/class_registry/class_registry.h"
#include "../engine/game_interface.h"
#include "../xenity_editor.h"
#include "ui/menus/compiling_menu.h"
#include <filesystem>
#include "../engine/scene_management/scene_manager.h"
#include "../engine/engine_settings.h"

std::string tempCompileFolderPath = "";

std::string WindowsPathToWSL(const std::string& path) 
{
	std::string newPath = path;
	newPath[0] = tolower(newPath[0]);
	int pathSize = path.size();
	for (int i = 1; i < pathSize; i++)
	{
		if (newPath[i] == '\\') 
		{
			newPath[i] = '/';
		}
	}
	newPath.erase(newPath.begin() + 1);
	return newPath;
}

CompileResult Compiler::CompileInWSL(Platform platform, const std::string& exportPath)
{
	std::string convertedEnginePath = WindowsPathToWSL(EngineSettings::engineProjectPath);
	// Clear compilation folder
	int clearFolderResult = system("wsl sh -c 'rm -rf ~/XenityTestProject'");

	// Create folders
	int createProjectFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject'");
	int createBuildFolderResult = system("wsl sh -c 'mkdir ~/XenityTestProject/build'");

	// Copy files
	std::string copyEngineSourceCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "Source ~/XenityTestProject'";
	int copyCodeResult = system(copyEngineSourceCommand.c_str()); // Engine's source code + (game's code but to change later)
	std::string copyEngineLibrariesCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "include ~/XenityTestProject'";
	int copyLibrariesResult = system(copyEngineLibrariesCommand.c_str()); // Engine's libraries
	std::string copyCmakeCommand = "wsl sh -c 'cp -R /mnt/" + convertedEnginePath + "CMakeLists.txt ~/XenityTestProject'";
	int copyCmakelistsResult = system(copyCmakeCommand.c_str()); // Cmakelists file

	if (copyCodeResult != 0)
	{
		return CompileResult::ERROR_WSL_ENGINE_CODE_COPY;
	}
	else if (copyLibrariesResult != 0)
	{
		return CompileResult::ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY;
	}
	else if (copyCmakelistsResult != 0)
	{
		return CompileResult::ERROR_WSL_CMAKELISTS_COPY;
	}

	// get the thread number of the cpu
	unsigned int threadNumber = std::thread::hardware_concurrency();
	if (threadNumber == 0) // This function may returns 0, use 1 instead
	{
		threadNumber = 1;
	}

	std::string compileCommand = "wsl bash -c -i \"cd ~/XenityTestProject/build";
	if (platform == Platform::PSP)
		compileCommand += " && psp-cmake -DMODE=psp ..";
	else if (platform == Platform::PsVita)
		compileCommand += " && cmake -DMODE=psvita ..";

	compileCommand += " && cmake --build . -j" + std::to_string(threadNumber) + "\""; // Use thread number to increase compilation speed

	// Start compilation
	int compileResult = system(compileCommand.c_str());
	if (compileResult != 0)
	{
		return CompileResult::ERROR_WSL_COMPILATION;
	}

	std::string compileFolderPath = exportPath;
	compileFolderPath = compileFolderPath.erase(1, 1);
	size_t pathSize = compileFolderPath.size();
	for (size_t i = 0; i < pathSize; i++)
	{
		if (compileFolderPath[i] == '\\')
		{
			compileFolderPath[i] = '/';
		}
	}
	compileFolderPath[0] = tolower(compileFolderPath[0]);
	compileFolderPath = "/mnt/" + compileFolderPath;
	std::string copyGameCommand;
	if (platform == Platform::PSP)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/EBOOT.PBP\" \"" + compileFolderPath + "/EBOOT.PBP\"'";
	else if (platform == Platform::PsVita)
		copyGameCommand = "wsl sh -c 'cp ~/\"XenityTestProject/build/hello.vpk\" \"" + compileFolderPath + "/hello.vpk\"'";

	int copyGameResult = system(copyGameCommand.c_str());
	if (copyGameResult != 0)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	// Copy game assets
	try
	{
		std::filesystem::copy(ProjectManager::GetAssetFolderPath(), exportPath + "assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		std::filesystem::copy(ProjectManager::GetEngineAssetFolderPath(), exportPath + "engine_assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
		std::filesystem::copy_file(ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME, std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::exception&)
	{
		return CompileResult::ERROR_FINAL_GAME_FILES_COPY;
	}

	return CompileResult::SUCCESS;
}

std::string Compiler::GetStartCompilerCommand()
{
	std::string command;
	command += EngineSettings::compilerPath.substr(0,2); // Go to the compiler folder
	command += " && cd " + EngineSettings::compilerPath; // Go to the compiler folder
	command += " && vcvarsamd64_x86.bat"; // Start the compiler
	//command += " >nul";	// Mute output
	return command;
}

std::string Compiler::GetAddNextCommand()
{
	std::string command = " && ";
	return command;
}

std::string Compiler::GetNavToEngineFolderCommand()
{
	std::string command = "";
	command += tempCompileFolderPath.substr(0, 2); // Change current drive
	command += " && cd " + tempCompileFolderPath;
	return command;
}

std::string Compiler::GetCompileGameLibCommand(BuildType buildType)
{
	std::string command = "";
	command += "cl /std:c++20 /MP /EHsc /DIMPORT"; // Start compilation
	if (buildType == BuildType::EditorHotReloading)
	{
		command += " /DEDITOR";
	}
	std::string folder = tempCompileFolderPath + "source\\";
	command += " -I \"" + std::string(EngineSettings::engineProjectPath) + "include\" /LD \"" + folder + "*.cpp\"";
	if (buildType != BuildType::EditorHotReloading)
	{
		command += " engine_game.lib";
	}
	else
	{
		command += " engine_editor.lib";
	}
	command += " /link";
	if (buildType != BuildType::EditorHotReloading)
	{
		command += " /implib:game.lib /out:game.dll";
	}
	else
	{
		command += " /implib:game_editor.lib /out:game_editor.dll";
	}
	//command += " >nul"; // Mute output
	return command;
}

std::string Compiler::GetCompileGameExeCommand()
{
	std::string command;
	std::string fileName = ProjectManager::GetGameName();
	command = "cl /Fe\"" + fileName + ".exe\" /std:c++20 /MP /EHsc -I \"" + std::string(EngineSettings::engineProjectPath) + "include\" main.cpp engine_game.lib"; //Buid game exe

	//command += " >nul"; // Mute output
	return command;
}

void Compiler::CompileGameThreaded(Platform platform, BuildType buildType, const std::string& exportPath)
{
	Editor::compilingMenu->OpenPopup();
	std::thread t = std::thread(CompileGame, platform, buildType, exportPath);
	t.detach();
}

void Compiler::OnCompileEnd(CompileResult result)
{
	switch (result)
	{
	case CompileResult::SUCCESS:
		Debug::Print("Game compiled successfully!");
		break;
	case CompileResult::ERROR_UNKNOWN:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	case CompileResult::ERROR_ENGINE_GAME_LIB_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing engine_game.lib");
		break;
	case CompileResult::ERROR_ENGINE_EDITOR_LIB_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing engine_editor.lib");
		break;
	case CompileResult::ERROR_LIB_DLLS_MISSING:
		Debug::PrintError("[Compiler::OnCompileEnd] Missing one of Dlls");
		break;
	case CompileResult::ERROR_ENGINE_HEADERS_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine headers");
		break;
	case CompileResult::ERROR_GAME_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's code");
		break;
	case CompileResult::ERROR_FINAL_GAME_FILES_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying game's files");
		break;

		// Specific to WSL
	case CompileResult::ERROR_WSL_COMPILATION:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile on WSL");
		break;
	case CompileResult::ERROR_WSL_ENGINE_CODE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine's code");
		break;
	case CompileResult::ERROR_WSL_ENGINE_LIBS_INCLUDE_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying engine's libraries files");
		break;
	case CompileResult::ERROR_WSL_CMAKELISTS_COPY:
		Debug::PrintError("[Compiler::OnCompileEnd] Error when copying CMakeLists.txt file");
		break;

	default:
		Debug::PrintError("[Compiler::OnCompileEnd] Unable to compile (unkown error)");
		break;
	}
	Editor::compilingMenu->ClosePopup();
}

void Compiler::CompileGame(Platform platform, BuildType buildType, const std::string& exportPath)
{
	// Set the folder path to use for the compilation
	if (buildType == BuildType::EditorHotReloading)
	{
		tempCompileFolderPath = ProjectManager::GetProjectFolderPath() + "hot_reloading_data\\";
	}
	else
	{
		tempCompileFolderPath = ProjectManager::GetProjectFolderPath() + "temp_build\\";
	}

	// Delete the directory and create a new one
	try
	{
		std::filesystem::remove_all(tempCompileFolderPath);
		std::filesystem::create_directory(tempCompileFolderPath);
	}
	catch (const std::exception&)
	{
		Debug::PrintWarning("[Compiler::CompileGame] Unable to clear the compilation folder");
	}

	if (platform == Platform::Windows)
	{
		if (buildType == BuildType::EditorHotReloading) // In hot reloading mode:
		{
			std::string engineLibPath = EngineSettings::engineProjectPath + "engine_editor.lib";

			try
			{
				// Copy engine editor lib to the temp build folder
				std::filesystem::copy_file(engineLibPath, tempCompileFolderPath + "engine_editor.lib", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(CompileResult::ERROR_ENGINE_EDITOR_LIB_MISSING);
				return;
			}

			// Copy engine headers
			try
			{
				std::filesystem::copy(EngineSettings::engineProjectPath + "Source\\engine\\", tempCompileFolderPath + "engine\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				std::filesystem::copy(EngineSettings::engineProjectPath + "Source\\editor\\", tempCompileFolderPath + "editor\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				std::filesystem::copy_file(EngineSettings::engineProjectPath + "Source\\xenity.h", tempCompileFolderPath + "xenity.h", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(EngineSettings::engineProjectPath + "Source\\xenity_editor.h", tempCompileFolderPath + "xenity_editor.h", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(CompileResult::ERROR_ENGINE_HEADERS_COPY);
				return;
			}
		}
		else // In game build mode:
		{
			std::string engineLibPath = EngineSettings::engineProjectPath + "engine_game.lib";
			std::string engineDllPath = EngineSettings::engineProjectPath + "engine_game.dll";
			std::string sdlDllPath = EngineSettings::engineProjectPath + "SDL2.dll";
			std::string glfwDllPath = EngineSettings::engineProjectPath + "glfw3.dll";

			// Copy engine game lib to the temp build folder
			try
			{
				std::filesystem::copy_file(engineLibPath, tempCompileFolderPath + "engine_game.lib", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(CompileResult::ERROR_ENGINE_GAME_LIB_MISSING);
				return;
			}

			// Copy all DLLs to the export folder
			try
			{
				std::filesystem::copy_file(engineDllPath, exportPath + "engine_game.dll", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(sdlDllPath, exportPath + "SDL2.dll", std::filesystem::copy_options::overwrite_existing);
				std::filesystem::copy_file(glfwDllPath, exportPath + "glfw3.dll", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(CompileResult::ERROR_LIB_DLLS_MISSING);
				return;
			}

			// Copy engine headers to the temp build folder
			try
			{
				std::filesystem::copy(EngineSettings::engineProjectPath + "Source\\engine\\", tempCompileFolderPath + "engine\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				std::filesystem::copy_file(EngineSettings::engineProjectPath + "Source\\xenity.h", tempCompileFolderPath + "xenity.h", std::filesystem::copy_options::overwrite_existing);

				std::filesystem::copy_file(EngineSettings::engineProjectPath + "Source\\main.cpp", tempCompileFolderPath + "main.cpp", std::filesystem::copy_options::overwrite_existing);
			}
			catch (const std::exception&)
			{
				OnCompileEnd(CompileResult::ERROR_ENGINE_HEADERS_COPY);
				return;
			}
		}

		// Copy game code
		try
		{
			std::filesystem::create_directory(tempCompileFolderPath + "source\\");

			for (const auto& file : std::filesystem::directory_iterator(ProjectManager::GetAssetFolderPath()))
			{
				if (file.is_regular_file())
				{
					std::string ext = file.path().extension().string();
					if (ext == ".h" || ext == ".cpp")
					{
						std::string path = file.path().string();
						std::string fileName = file.path().filename().string();
						std::filesystem::copy_file(path, tempCompileFolderPath + "source\\" + fileName, std::filesystem::copy_options::overwrite_existing);
					}
				}
			}
		}
		catch (const std::exception&)
		{
			OnCompileEnd(CompileResult::ERROR_GAME_CODE_COPY);
			return;
		}

		// Setup windows compileur command
		std::string command;
		command = GetStartCompilerCommand();
		command += GetAddNextCommand();
		command += GetNavToEngineFolderCommand();
		command += GetAddNextCommand();
		command += GetCompileGameLibCommand(buildType);
		if (buildType != BuildType::EditorHotReloading)
		{
			command += GetAddNextCommand();
			command += GetCompileGameExeCommand();
		}

		int buildResult = system(command.c_str());
		if (buildResult == 0)
		{
			try
			{
				if (buildType == BuildType::EditorHotReloading)
				{
					std::filesystem::copy_file(tempCompileFolderPath + "game_editor.dll", ProjectManager::GetProjectFolderPath() + "\\game_editor.dll", std::filesystem::copy_options::overwrite_existing);
				}
				else
				{
					std::filesystem::copy_file(tempCompileFolderPath + ProjectManager::GetGameName() + ".exe", exportPath + ProjectManager::GetGameName() + ".exe", std::filesystem::copy_options::overwrite_existing);
					std::filesystem::copy_file(tempCompileFolderPath + "game.dll", exportPath + "game.dll", std::filesystem::copy_options::overwrite_existing);
					std::filesystem::copy(ProjectManager::GetAssetFolderPath(), exportPath + "assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
					std::filesystem::copy(ProjectManager::GetEngineAssetFolderPath(), exportPath + "engine_assets\\", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
					std::filesystem::copy_file(ProjectManager::GetProjectFolderPath() + PROJECT_SETTINGS_FILE_NAME, exportPath + PROJECT_SETTINGS_FILE_NAME, std::filesystem::copy_options::overwrite_existing);
				}
			}
			catch (const std::exception&)
			{
				OnCompileEnd(CompileResult::ERROR_FINAL_GAME_FILES_COPY);
				return;
			}

			// Delete temp compiler folder content
			try
			{
				std::filesystem::remove_all(tempCompileFolderPath);
			}
			catch (const std::exception&)
			{
			}

			if (buildType == BuildType::BuildAndRunGame)
			{
				auto t = std::thread(StartGame, platform, exportPath);
				t.detach();
			}
		}
		else
		{
			OnCompileEnd(CompileResult::ERROR_UNKNOWN);
			return;
		}
	}
	else
	{
		CompileResult result = CompileInWSL(platform, exportPath);
		if (result == CompileResult::SUCCESS && buildType == BuildType::BuildAndRunGame)
		{
			auto t = std::thread(StartGame, platform, exportPath);
			t.detach();
		}
		else
		{
			OnCompileEnd(result);
			return;
		}
	}

	OnCompileEnd(CompileResult::SUCCESS);
}

void Compiler::StartGame(Platform platform, const std::string& exportPath)
{
	if (platform == Platform::Windows)
	{
		std::string fileName = ProjectManager::GetGameName();
		std::string command = "cd \"" + exportPath + "\"" + " && " + "\"" + fileName + ".exe\"";

		system(command.c_str());
	}
	else if (platform == Platform::PSP)
	{
		std::string command = "(\"C:\\Program Files\\PPSSPP\\PPSSPPWindows.exe\" \"" + exportPath + "EBOOT.PBP\")";
		system(command.c_str());
	}
}

void Compiler::HotReloadGame()
{
#if defined(_WIN32) || defined(_WIN64)
	Engine::game.reset();
	Engine::game = nullptr;

	SceneManager::SaveScene(SaveSceneForHotReloading);
	SceneManager::ClearScene();

	ClassRegistry::Reset();
	Engine::RegisterEngineComponents();
	DynamicLibrary::UnloadGameLibrary();
	Compiler::CompileGame(Platform::Windows, BuildType::EditorHotReloading, "");
	DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "game_editor");
	Engine::game = DynamicLibrary::CreateGame();
	if (Engine::game)
	{
		Debug::Print("Game compilation done");
		Engine::game->Start();
		SceneManager::RestoreSceneHotReloading();
	}
	else
	{
		Debug::PrintError("[Compiler::HotReloadGame] Game compilation failed");
		SceneManager::RestoreSceneHotReloading();
	}
#endif
}