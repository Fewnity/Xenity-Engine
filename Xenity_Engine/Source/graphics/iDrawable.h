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
	//IDrawableTypes type;
	virtual int GetDrawPriority() = 0;
	bool invertedTriangles = false;
};

