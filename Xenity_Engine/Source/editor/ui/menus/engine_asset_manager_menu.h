#pragma once

#include "menu.h"
#include <memory>

class File;

class EngineAssetManagerMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	std::vector<std::shared_ptr<File>> engineAssetsFiles;
	std::vector<uint64_t> ids;
	std::vector<uint64_t> oldIds;
};

