#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <vector>
#include <engine/vectors/vector3.h>
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
	/**
	* Create a spline point
	* @param position Position of the point
	*/
	SplinePoint* CreateSplinePoint(const Vector3& position);

	/**
	* Add a point to the spline
	* @param point Point of the spline
	*/
	void AddSplinePoint(SplinePoint* point);

	/**
	* Get spline value at t
	* @param t [0;1]
	*/
	Vector3 GetValueAt(const float t) const;
private:
	std::vector<SplinePoint*> splinePoints;
};
