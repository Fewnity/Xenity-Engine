#pragma once

#include <engine/component.h>
#include <engine/vectors/vector2.h>

class Canvas;

class RectTransform : public Component
{
public:
	RectTransform();
	~RectTransform();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;
	Vector2 position = Vector2(0);
	Vector2 anchors = Vector2(0);
	void UpdatePosition(std::shared_ptr <Canvas> canvas);
	void UpdatePosition(std::shared_ptr <RectTransform> canvas);

private:
};