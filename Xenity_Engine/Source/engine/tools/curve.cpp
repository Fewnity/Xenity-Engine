#include "curve.h"

#include "../game_elements/transform.h"
#include "../game_elements/gameobject.h"
#include <iostream>

Vector3 Spline::GetValueAt(const float t) const
{
    int curveCount = (int)splinePoints.size() - 1;
    int currentCurve = (int)floorf(t * curveCount);
    if (currentCurve == curveCount)
        currentCurve--;

    float tVal = t * curveCount;
    tVal -= currentCurve;

    Vector3 result = Vector3();

    result.x = powf(1 - tVal, 3) * splinePoints[0 + currentCurve]->parent->GetPosition().x
        + 3 * powf((1 - tVal), 2) * tVal * splinePoints[0 + currentCurve]->next->GetPosition().x
        + 3 * (1 - tVal) * powf(tVal, 2) * splinePoints[1 + currentCurve]->before->GetPosition().x
        + powf(tVal, 3) * splinePoints[1 + currentCurve]->parent->GetPosition().x;

    result.y = powf(1 - tVal, 3) * splinePoints[0 + currentCurve]->parent->GetPosition().y
        + 3 * powf((1 - tVal), 2) * tVal * splinePoints[0 + currentCurve]->next->GetPosition().y
        + 3 * (1 - tVal) * powf(tVal, 2) * splinePoints[1 + currentCurve]->before->GetPosition().y
        + powf(tVal, 3) * splinePoints[1 + currentCurve]->parent->GetPosition().y;

    result.z = powf(1 - tVal, 3) * splinePoints[0 + currentCurve]->parent->GetPosition().z
        + 3 * powf((1 - tVal), 2) * tVal * splinePoints[0 + currentCurve]->next->GetPosition().z
        + 3 * (1 - tVal) * powf(tVal, 2) * splinePoints[1 + currentCurve]->before->GetPosition().z
        + powf(tVal, 3) * splinePoints[1 + currentCurve]->parent->GetPosition().z;

	return result;
}

SplinePoint* Spline::CreateSplinePoint(const Vector3 position)
{
    SplinePoint* point = new SplinePoint();

    GameObject* parent = new GameObject();
    parent->GetTransform()->SetPosition(position);

    GameObject* next = new GameObject();
    parent->AddChild(next);
    next->GetTransform()->SetLocalPosition(Vector3(0.5f, 0, 0));

    GameObject* before = new GameObject();
    parent->AddChild(before);
    before->GetTransform()->SetLocalPosition(Vector3(-0.5f, 0, 0));

    point->parent = parent->GetTransform();
    point->next = next->GetTransform();
    point->before = before->GetTransform();

    return point;
}

void Spline::AddSplinePoint(SplinePoint* point)
{
    splinePoints.push_back(point);
}