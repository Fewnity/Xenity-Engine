#pragma once
#include "../asset_manager.h"

class IDrawable
{
public:

	IDrawable();

	virtual ~IDrawable();

	virtual void Draw() {};
	int drawPriority = 0;
};

