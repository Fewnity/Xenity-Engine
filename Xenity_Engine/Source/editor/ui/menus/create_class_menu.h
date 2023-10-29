#pragma once

#include "menu.h"
#include <memory>
#include <string>

class CreateClassMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void Reset();
	std::string className = "";
	std::string fileName = "";
	std::string filePath = "";
private:
	bool fileNameChanged = false;
};

