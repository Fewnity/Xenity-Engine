#pragma once

class Menu
{
public:
	virtual void Init() = 0;
	virtual void Draw() = 0;
	void Focus();
protected:
	bool forceFocus = false;
};

