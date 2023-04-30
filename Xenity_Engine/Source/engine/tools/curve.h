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
    SplinePoint* CreateSplinePoint(const Vector3 position);
    void AddSplinePoint(SplinePoint* point);
    Vector3 GetValueAt(const float t) const;

    std::vector<SplinePoint*> splinePoints;
};
