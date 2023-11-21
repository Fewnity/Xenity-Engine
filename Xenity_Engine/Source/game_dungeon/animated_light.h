#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class Light;

class AnimatedLight : public MonoBehaviour
{
public:
	AnimatedLight();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
	std::weak_ptr<Light> light;
	float speed = 2;
	float startValue = 2;
	float range = 1;

private:
};
