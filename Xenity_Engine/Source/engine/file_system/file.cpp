#include "file.h"
#include <string>
#include "stb_image.h"
#include <iostream>
#include <fstream>

#include "../engine_settings.h"
#include "../debug/debug.h"

std::string File::texturePath = R"(Xenity_Engine\Source\images\)";
std::string File::shaderPath = R"(Xenity_Engine\Source\shaders\)";
std::string File::modelsPath = R"(Xenity_Engine\Source\models\)";

#pragma region Read/Input

unsigned char* File::LoadTextureData(const std::string filePath, int& width, int& height, int& nrChannels)
{
	unsigned char* data = stbi_load((texturePath + filePath).c_str(), &width, &height, &nrChannels, 0);
	return data;
}


std::string File::ReadText(const std::string path)
{
	//Open file
	std::ifstream file;
	//file.open(finalpath + path);
	file.open(path);

	//Print error if the file can't be read
	if (file.fail())
	{
		std::cout << "\033[31mShader read error. Path : \"" << path << "\"\033[0m" << std::endl;
	}

	//Read file
	std::string text = "", line;
	while (getline(file, line))
	{
		text += line + '\n';
	}

	//Close the file
	file.close();

	return text;
}

#pragma endregion

#pragma region Write/Output

#pragma endregion


void File::InitFileSystem(const std::string exePath)
{
	//EngineSettings::RootFolder = R"(C:\Users\gregory.machefer\Documents\GitHub\Xenity-Engine\)";
	//EngineSettings::RootFolder = R"(C:\Users\elect\Documents\GitHub\Xenity-Engine\)";
	
	//EngineSettings::RootFolder = exePath;
	int index = (int)exePath.find(R"(\Xenity-Engine\)");
	EngineSettings::RootFolder = exePath.substr(0, index + 15);

	texturePath = EngineSettings::RootFolder + texturePath;
	shaderPath = EngineSettings::RootFolder + shaderPath;
	modelsPath = EngineSettings::RootFolder + modelsPath;

	Debug::Print("---- File System initiated ----");
	//gamePath += R"(Debug\)"; //TODO remove this
}

std::string File::GetGamePath() 
{
	return EngineSettings::RootFolder;
}