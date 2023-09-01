#pragma once

#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#include "../file_system/file_reference.h"
#include <string>

class API Scene : public FileReference, public Reflection
{
public:
	Scene();
	std::unordered_map<std::string, Variable> GetReflection();
	std::unordered_map<std::string, Variable> GetMetaReflection();
	static std::shared_ptr<Scene> MakeScene();
private:
};

