#pragma once

#include "menu.h"

class CompilerParams;

enum class CompilingPupopState
{
	Closed,
	Closing,
	Opening
};

class CompilingMenu : public Menu
{
public:
	~CompilingMenu();
	void Init() override;
	void Draw() override;
	void OpenPopup(CompilerParams params);
	void ClosePopup(CompilerParams params, bool result);

private:
	CompilingPupopState popupState = CompilingPupopState::Closed;
};

