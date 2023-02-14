#pragma once

class IDrawable
{
public:
	virtual void Draw() {};
	int drawPriority = 0;
};

