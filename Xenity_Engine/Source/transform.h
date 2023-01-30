#pragma once

#include "vectors/vector3.h"

class Transform
{
public:
	Vector3 position = Vector3();
	Vector3 rotation = Vector3();//Euler angle
	Vector3 scale = Vector3(1);
};

