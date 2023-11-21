#include "animated_light.h"
#include "../xenity.h"

AnimatedLight::AnimatedLight()
{
	componentName = "AnimatedLight";
}

void AnimatedLight::Start()
{
	if (light.lock())
		startValue = light.lock()->GetIntensity();
}

void AnimatedLight::Update()
{
	light.lock()->SetIntensity(startValue + (cos(Time::GetTime() * speed) * 0.5f * range));
}

ReflectiveData AnimatedLight::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, light, "light", true);
	Reflective::AddVariable(reflectedVariables, speed, "speed", true);
	Reflective::AddVariable(reflectedVariables, range, "range", true);
	Reflective::AddVariable(reflectedVariables, startValue, "startValue", false);
	return reflectedVariables;
}
