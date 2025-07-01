#pragma once

#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/ui/image_renderer.h>
#include <engine/event_system/event_system.h>

class Texture;
class Canvas;

/**
* @brief Component that renders a button on the screen.
*/
class API Button : public ImageRenderer
{
public:
	/**
	* @brief Get the onClick event
	*/
	Event<>& GetOnClickEvent()
	{
		return onClick;
	}

private:
	friend class Canvas;

	void CheckClick(Canvas& canvas);
	Event<> onClick;
};

