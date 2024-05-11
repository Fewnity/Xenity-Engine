#pragma once

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class Icon : public FileReference, public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	static std::shared_ptr<Icon> MakeIcon();
};

