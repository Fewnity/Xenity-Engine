#include "fps_counter.h"

#include <engine/time/time.h>
#include <engine/graphics/ui/text_renderer.h>

void FpsCounter::Update()
{
	// Basic FPS average calculation
	m_lastFps += 1 / Time::GetUnscaledDeltaTime();
	m_lastFps /= 2.0f;

	m_updateCounter -= Time::GetUnscaledDeltaTime();

	const std::shared_ptr<TextRenderer> textRendererLock = m_textRenderer.lock();
	if (textRendererLock)
	{
		if (m_updateCounter <= 0 || !showAverage)
		{
			m_updateCounter = 0.1f;

			// Simplify the fps string to avoid too many decimals
			std::string fpsString = "";
			if (showAverage)
			{
				fpsString = std::to_string(m_lastFps);
			}
			else
			{
				fpsString = std::to_string(1 / Time::GetUnscaledDeltaTime());
			}

			const size_t dotPos = fpsString.find(".");
			const std::string fpsShortString = fpsString.substr(0, dotPos + 2);
			textRendererLock->SetText("FPS: " + fpsShortString);
		}
	}
}

ReflectiveData FpsCounter::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_textRenderer, "textRenderer");
	Reflective::AddVariable(reflectedVariables, showAverage, "showAverage");
	return reflectedVariables;
}
