#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class TextRenderer;

class FpsCounter : public MonoBehaviour
{
public:
	FpsCounter();
	void Start() override;
	void Update() override;
	std::weak_ptr<TextRenderer> textRenderer;
	ReflectiveData GetReflectiveData() override;
private:
};

