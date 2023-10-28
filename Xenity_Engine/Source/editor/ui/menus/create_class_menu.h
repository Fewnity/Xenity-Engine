#pragma once

#include "menu.h"
#include <memory>
#include <string>

class CreateClassMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	std::string className = "";
	std::string filePath = "";
private:
};

