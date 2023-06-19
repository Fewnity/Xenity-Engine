#pragma once

#include <memory>
#include "../component.h"
#include "iDrawableTypes.h"

class IDrawable : public Component
{
public:
	IDrawable();

	virtual ~IDrawable();

	virtual void Draw(){};
	IDrawableTypes type = Draw_3D;
	virtual int GetDrawPriority() const = 0;
};
