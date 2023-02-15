#include "iDrawable.h"

IDrawable::IDrawable() 
{
	AssetManager::AddDrawable(this);
}

IDrawable::~IDrawable() 
{
	AssetManager::RemoveDrawable(this);
}