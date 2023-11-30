#include "iDrawable.h"
#include "graphics.h"

IDrawable::IDrawable()
{
}

IDrawable::~IDrawable()
{
}

void IDrawable::RemoveReferences()
{
	Graphics::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(shared_from_this()));
}
