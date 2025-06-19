#pragma once

#include <engine/component.h>

class TextRenderer;

/**
* @brief Component to display the FPS counter in the game
*/
class API FpsCounter : public Component
{
	bool showAverage = true;

	/**
	* @brief Get the text renderer used for displaying the FPS
	*/
	const std::weak_ptr<TextRenderer>& GetTextRenderer()
	{
		return m_textRenderer;
	}

	/**
	* @brief Set the text renderer to use for displaying the FPS
	*/
	void SetTextRenderer(const std::shared_ptr<TextRenderer>& textRenderer)
	{
		m_textRenderer = textRenderer;
	}

private:
	void Update() override;
	ReflectiveData GetReflectiveData() override;

	std::weak_ptr<TextRenderer> m_textRenderer;
	float m_updateCounter = 0.1f;
	float m_lastFps = 0;
};

