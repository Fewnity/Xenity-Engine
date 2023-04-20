#pragma once
#include "../asset_manager.h"
#include "../component.h"
#include "iDrawableTypes.h"

class IDrawable : public Component
{
public:

	IDrawable();

	virtual ~IDrawable();

	virtual void Draw() {};
	int drawPriority = 0;
	//IDrawableTypes type;
	bool invertedTriangles = false;
};

