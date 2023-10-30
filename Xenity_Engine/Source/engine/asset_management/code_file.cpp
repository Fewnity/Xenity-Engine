#include "code_file.h"
#include "asset_manager.h"

CodeFile::CodeFile(const bool isHeader)
{
	this->isHeader = isHeader;
}

std::shared_ptr<CodeFile> CodeFile::MakeCode(const bool isHeader)
{
	std::shared_ptr<CodeFile> newFileRef = std::make_shared<CodeFile>(isHeader);
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

std::unordered_map<std::string, ReflectionEntry> CodeFile::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

std::unordered_map<std::string, ReflectionEntry> CodeFile::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}