#include "fps_counter.h"

#include <engine/time/time.h>
#include <engine/graphics/ui/text_renderer.h>

void FpsCounter::Update()
{
	// Basic FPS average calculation
	lastFps += 1 / Time::GetUnscaledDeltaTime();
	lastFps /= 2.0f;

	updateCounter -= Time::GetUnscaledDeltaTime();

	const std::shared_ptr<TextRenderer> textRendererLock = textRenderer.lock();
	if (textRendererLock)
	{
		if (updateCounter <= 0 || !showAverage)
		{
			updateCounter = 0.1f;

			// Simplify the fps string to avoid too many decimals
			std::string fpsString = "";
			if (showAverage)
			{
				fpsString = std::to_string(lastFps);
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
	Reflective::AddVariable(reflectedVariables, textRenderer, "textRenderer", true);
	Reflective::AddVariable(reflectedVariables, showAverage, "showAverage", true);
	return reflectedVariables;
}
