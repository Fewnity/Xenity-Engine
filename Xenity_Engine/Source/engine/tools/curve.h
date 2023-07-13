#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <vector>
#include "../vectors/vector3.h"
#include <memory>

class Transform;
class GameObject;

class API SplinePoint
{
public:
    std::weak_ptr<Transform> parent;
    std::weak_ptr<Transform> before;
    std::weak_ptr<Transform> next;
};

class API Spline
{
public:
    SplinePoint* CreateSplinePoint(const Vector3 position);
    void AddSplinePoint(SplinePoint* point);
    Vector3 GetValueAt(const float t) const;

    std::vector<SplinePoint*> splinePoints;
};
