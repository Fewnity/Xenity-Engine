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

    auto parentTransform = splinePoints[0 + currentCurve]->parent.lock();
    auto nextTransform = splinePoints[0 + currentCurve]->next.lock();
    auto beforeTransform = splinePoints[1 + currentCurve]->before.lock();
    auto parent2Transform =  splinePoints[1 + currentCurve]->parent.lock();

    result.x = powf(1 - tVal, 3) * parentTransform->GetPosition().x + 3 * powf((1 - tVal), 2) * tVal * nextTransform->GetPosition().x + 3 * (1 - tVal) * powf(tVal, 2) * beforeTransform->GetPosition().x + powf(tVal, 3) * parent2Transform->GetPosition().x;
    result.y = powf(1 - tVal, 3) * parentTransform->GetPosition().y + 3 * powf((1 - tVal), 2) * tVal * nextTransform->GetPosition().y + 3 * (1 - tVal) * powf(tVal, 2) * beforeTransform->GetPosition().y + powf(tVal, 3) * parent2Transform->GetPosition().y;
    result.z = powf(1 - tVal, 3) * parentTransform->GetPosition().z + 3 * powf((1 - tVal), 2) * tVal * nextTransform->GetPosition().z + 3 * (1 - tVal) * powf(tVal, 2) * beforeTransform->GetPosition().z + powf(tVal, 3) * parent2Transform->GetPosition().z;

    return result;
}

SplinePoint *Spline::CreateSplinePoint(const Vector3 position)
{
    SplinePoint *point = new SplinePoint();

    auto parent = CreateGameObject();
    parent->GetTransform()->SetPosition(position);

    auto next = CreateGameObject();
    parent->AddChild(next);
    next->GetTransform()->SetLocalPosition(Vector3(0.5f, 0, 0));

    auto before = CreateGameObject();
    parent->AddChild(before);
    before->GetTransform()->SetLocalPosition(Vector3(-0.5f, 0, 0));

    point->parent = parent->GetTransform();
    point->next = next->GetTransform();
    point->before = before->GetTransform();

    return point;
}

void Spline::AddSplinePoint(SplinePoint *point)
{
    splinePoints.push_back(point);
}