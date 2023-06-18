#include "curve.h"

#include "../game_elements/transform.h"
#include "../game_elements/gameobject.h"
#include "../engine.h"

Vector3 Spline::GetValueAt(const float t) const
{
    int curveCount = (int)splinePoints.size() - 1;
    int currentCurve = (int)floorf(t * curveCount);
    if (currentCurve == curveCount)
        currentCurve--;

    float tVal = t * curveCount;
    tVal -= currentCurve;

    Vector3 result = Vector3();

    result.x = powf(1 - tVal, 3) * splinePoints[0 + currentCurve]->parent.lock()->GetPosition().x + 3 * powf((1 - tVal), 2) * tVal * splinePoints[0 + currentCurve]->next.lock()->GetPosition().x + 3 * (1 - tVal) * powf(tVal, 2) * splinePoints[1 + currentCurve]->before.lock()->GetPosition().x + powf(tVal, 3) * splinePoints[1 + currentCurve]->parent.lock()->GetPosition().x;
    result.y = powf(1 - tVal, 3) * splinePoints[0 + currentCurve]->parent.lock()->GetPosition().y + 3 * powf((1 - tVal), 2) * tVal * splinePoints[0 + currentCurve]->next.lock()->GetPosition().y + 3 * (1 - tVal) * powf(tVal, 2) * splinePoints[1 + currentCurve]->before.lock()->GetPosition().y + powf(tVal, 3) * splinePoints[1 + currentCurve]->parent.lock()->GetPosition().y;
    result.z = powf(1 - tVal, 3) * splinePoints[0 + currentCurve]->parent.lock()->GetPosition().z + 3 * powf((1 - tVal), 2) * tVal * splinePoints[0 + currentCurve]->next.lock()->GetPosition().z + 3 * (1 - tVal) * powf(tVal, 2) * splinePoints[1 + currentCurve]->before.lock()->GetPosition().z + powf(tVal, 3) * splinePoints[1 + currentCurve]->parent.lock()->GetPosition().z;

    return result;
}

SplinePoint *Spline::CreateSplinePoint(const Vector3 position)
{
    SplinePoint *point = new SplinePoint();

    std::weak_ptr<GameObject> parent = CreateGameObject();
    parent.lock()->GetTransform().lock()->SetPosition(position);

    std::weak_ptr<GameObject>next = CreateGameObject();
    parent.lock()->AddChild(next);
    next.lock()->GetTransform().lock()->SetLocalPosition(Vector3(0.5f, 0, 0));

    std::weak_ptr<GameObject>before = CreateGameObject();
    parent.lock()->AddChild(before);
    before.lock()->GetTransform().lock()->SetLocalPosition(Vector3(-0.5f, 0, 0));

    point->parent = parent.lock()->GetTransform();
    point->next = next.lock()->GetTransform();
    point->before = before.lock()->GetTransform();

    return point;
}

void Spline::AddSplinePoint(SplinePoint *point)
{
    splinePoints.push_back(point);
}