#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class TextRendererCanvas;

class FpsCounter : public MonoBehaviour
{
public:
	FpsCounter();
	void Start() override;
	void Update() override;
	std::weak_ptr<TextRendererCanvas> textRenderer;
	ReflectiveData GetReflectiveData() override;
private:
};

