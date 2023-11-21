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

ReflectiveData CodeFile::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData CodeFile::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}