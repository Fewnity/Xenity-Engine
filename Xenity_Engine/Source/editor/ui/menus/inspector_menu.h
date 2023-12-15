#pragma once

#include "menu.h"
#include <memory>
#include <string>

class FileReference;
class Component;
class GameObject;

class InspectorMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	std::shared_ptr<FileReference> loadedPreview = nullptr;
	int CheckOpenRightClickPopupFile(std::shared_ptr<Component>& component, int& componentCount, int& componentIndex, const std::string& id);

private:
	void DrawFilePreview();
	void DrawFileInfo(std::shared_ptr<FileReference> & selectedFileReference);
	void DrawGameObjectInfo(std::shared_ptr <GameObject> selectedGameObject);
	void DrawTransformHeader(std::shared_ptr <GameObject> selectedGameObject);
	void DrawComponentsHeaders(std::shared_ptr <GameObject> selectedGameObject);

	std::string previewText = "";
};

