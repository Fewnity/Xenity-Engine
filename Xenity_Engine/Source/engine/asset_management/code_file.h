#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../file_system/file_reference.h"
#include "../reflection/reflection.h"

class CodeFile : public FileReference, public Reflective
{
public:
	CodeFile(const bool isHeader);
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflection() override;
	static std::shared_ptr<CodeFile> MakeCode(const bool isHeader);

	bool GetIsHeader() 
	{
		return isHeader;
	}

private:
	bool isHeader = false;
};

