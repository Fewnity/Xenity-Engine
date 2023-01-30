#include "file.h"
#include <string>
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include "../main.h"

std::string texturePath = R"(Xenity_Engine\Source\images\)"; //TODO improve this

unsigned char* File::LoadTextureData(std::string filePath, int& width, int& height, int& nrChannels) {
	std::string finalpath = PROJECT_FOLDER + texturePath;
	unsigned char* data = stbi_load((finalpath + filePath).c_str(), &width, &height, &nrChannels, 0);
	return data;
}

std::string shaderPath = R"(Xenity_Engine\Source\shaders\)"; //TODO improve this
//std::string gamePath = (PROJECT_FOLDER) + R"(C:\Users\gregory.machefer\Downloads\OpenGL_Test\Debug\)"; //TODO remove this

std::string File::LoadShaderData(const std::string path)
{
	std::string finalpath = PROJECT_FOLDER + shaderPath;
	//Open file
	std::ifstream file;
	file.open(finalpath + path);

	//Print error if the file can't be read
	if (file.fail()) {
		std::cout << "\033[31mShader read error. Path : \"" << finalpath + path << "\"\033[0m" << std::endl;
	}

	//Read file
	std::string text, line;
	while (getline(file, line)) 
	{
		text += line + '\n';
	}

	//Close the file
	file.close();

	return text;
}

std::string gamePath = PROJECT_FOLDER; //TODO improve this

void File::InitFileSystem() 
{
	gamePath += R"(Debug\)"; //TODO remove this
}

std::string File::GetGamePath() 
{
	return gamePath;
}