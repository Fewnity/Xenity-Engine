// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal] Class not visible to users
 */

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

class CodeFile : public FileReference
{
public:
	explicit CodeFile(const bool isHeader);

protected:
	friend class ProjectManager;

	[[nodiscard]] ReflectiveData GetReflectiveData() override;
	[[nodiscard]] ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	[[nodiscard]] static std::shared_ptr<CodeFile> MakeCode(const bool isHeader);

	/**
	* @brief Gets if the file is a header file
	* @return If the file is a header file
	*/
	[[nodiscard]] inline bool IsHeader()
	{
		return isHeader;
	}

	bool isHeader = false;
};

