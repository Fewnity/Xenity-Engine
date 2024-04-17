#pragma once

/**
 * [Internal]
 */

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class CodeFile : public FileReference, public Reflective
{
public:
	CodeFile(const bool isHeader);
	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	static std::shared_ptr<CodeFile> MakeCode(const bool isHeader);

	/**
	* @brief Gets if the file is a header file
	* @return If the file is a header file
	*/
	bool GetIsHeader() 
	{
		return isHeader;
	}

private:
	bool isHeader = false;
};

