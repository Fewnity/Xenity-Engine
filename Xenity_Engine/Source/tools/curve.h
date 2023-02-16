#pragma once

#include <vector>
#include "../vectors/vector3.h"

class Transform;
class GameObject;

class SplinePoint
{
public:
    Transform* parent;
    Transform* before;
    Transform* next;
};

class Spline
{
public:
    std::vector<SplinePoint*> splinePoints;
    Vector3 GetValueAt(float t);
    SplinePoint* CreateSplinePoint(Vector3 position);
    void AddSplinePoint(SplinePoint* point);
};
