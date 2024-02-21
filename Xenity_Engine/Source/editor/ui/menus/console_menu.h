#pragma once

#include <memory>
#include "menu.h"

class ConsoleMenu : public Menu
{
public:
	~ConsoleMenu();
	void Init() override;
	void Draw() override;
	void OnNewDebug();

private:
	bool consoleMode = true;
	bool showLogs = true;
	bool showWarnings = true;
	bool showErrors = true;
	int lastHistoryCount = 0;
	float maxScrollSize;
	int needUpdateScrool = 0;
};

