#pragma once

#include "menu.h"
#include <memory>
#include <string>

class FileReference;

class InspectorMenu : public Menu
{
public:
	void Init();
	void Draw();
	std::shared_ptr<FileReference> loadedPreview = nullptr;
private:
	void DrawFilePreview();
	std::string previewText = "";
};

