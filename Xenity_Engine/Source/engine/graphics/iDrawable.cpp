#include "iDrawable.h"

#include "../../xenity.h"

IDrawable::IDrawable()
{
	//AssetManager::AddDrawable(shared_from_this());
}

IDrawable::~IDrawable()
{
	//Graphics::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(shared_from_this())); 
	//AssetManager::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(shared_from_this()));
	//AssetManager::RemoveDrawable(std::make_shared<IDrawable>(shared_from_this()));
	//AssetManager::RemoveDrawable(shared_from_this());
}