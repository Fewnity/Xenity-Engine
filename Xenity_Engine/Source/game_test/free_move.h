#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class FreeMove : public MonoBehaviour
{
public:
	FreeMove();
	void Start() override;
	void Update() override;
	ReflectiveData GetReflectiveData() override;
private:
};

