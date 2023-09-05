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

class CodeFile : public FileReference, public Reflection
{
public:
	CodeFile(std::string fileExtension);
	std::unordered_map<std::string, ReflectionEntry> GetReflection();
	std::unordered_map < std::string, ReflectionEntry> GetMetaReflection();
	static std::shared_ptr<CodeFile> MakeScene(std::string fileExtension);
	bool isHeader = false;
};

