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

class API Scene : public FileReference, public Reflective
{
public:
	Scene();
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflection() override;
	static std::shared_ptr<Scene> MakeScene();
private:
};

