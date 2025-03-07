#pragma once

#include <engine/component.h>

class TextRenderer;

class FpsCounter : public Component
{
	bool showAverage = true;

private:
	void Update() override;
	ReflectiveData GetReflectiveData() override;

	std::weak_ptr<TextRenderer> textRenderer;
	float updateCounter = 0.1f;
	float lastFps = 0;
};

