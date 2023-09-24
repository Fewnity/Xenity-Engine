#pragma once

#include "menu.h"
#include <memory>
#include <string>

class FileReference;
class Component;

class InspectorMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	std::shared_ptr<FileReference> loadedPreview = nullptr;
	int CheckOpenRightClickPopupFile(std::shared_ptr<Component>& component, int& componentCount, int &componentIndex, const std::string& id);
private:
	void DrawFilePreview();
	std::string previewText = "";
};

