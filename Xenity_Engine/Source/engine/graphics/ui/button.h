#pragma once

#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/ui/image_renderer.h>
#include <engine/event_system/event_system.h>

class Texture;
class Canvas;

class API Button : public ImageRenderer
{
public:
	/**
	* @brief Get the image texture
	*/
	[[nodiscard]] const std::shared_ptr<Texture>& GetImage() const
	{
		return m_image;
	}

	/**
	* @brief Set the image texture
	*/
	void SetImage(const std::shared_ptr<Texture>& image);

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

