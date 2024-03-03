#pragma once

#include "menu.h"

#include <engine/reflection/reflection.h>
#include <editor/compiler.h>

class DockerConfigMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;
	void Refresh();
private:
	
	DockerState currentDockerState = DockerState::NOT_INSTALLED;
};

