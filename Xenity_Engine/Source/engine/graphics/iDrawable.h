#pragma once

#include "../component.h"
#include "iDrawableTypes.h"

class IDrawable : public Component
{
public:

	IDrawable();

	virtual ~IDrawable();

	virtual void Draw() {};
	//IDrawableTypes type;
	virtual int GetDrawPriority() const = 0;
};

