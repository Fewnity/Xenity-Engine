#include "code_file.h"
#include "asset_manager.h"

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

std::shared_ptr<CodeFile> CodeFile::MakeScene(std::string fileExtension)
{
	std::shared_ptr<CodeFile> newFileRef = std::make_shared<CodeFile>(fileExtension);
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
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