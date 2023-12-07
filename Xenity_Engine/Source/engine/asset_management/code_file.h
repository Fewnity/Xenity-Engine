#pragma once

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class CodeFile : public FileReference, public Reflective
{
public:
	CodeFile(const bool isHeader);
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<CodeFile> MakeCode(const bool isHeader);

	bool GetIsHeader() 
	{
		return isHeader;
	}

private:
	bool isHeader = false;
};

