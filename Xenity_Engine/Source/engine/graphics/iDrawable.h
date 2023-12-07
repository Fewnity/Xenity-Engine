#pragma once
#include <engine/api.h>

#include <memory>
#include <engine/component.h>
#include "iDrawableTypes.h"

class API IDrawable : public Component
{
public:
	IDrawable();

	virtual ~IDrawable();

	virtual void Draw() {};
	IDrawableTypes type = Draw_3D;
	virtual int GetDrawPriority() const = 0;

	void RemoveReferences() override;
};
