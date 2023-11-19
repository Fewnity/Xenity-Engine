#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class Rotate : public MonoBehaviour
{
public:
	Rotate();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
private:
};

