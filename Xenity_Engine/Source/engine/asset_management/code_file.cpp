#include "code_file.h"

CodeFile::CodeFile(std::string fileExtension)
{
	if (fileExtension == ".h") 
	{
		isHeader = true;
	}
	else 
	{
		isHeader = false;
	}
}

std::unordered_map<std::string, Variable> CodeFile::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}

std::unordered_map<std::string, Variable> CodeFile::GetMetaReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}