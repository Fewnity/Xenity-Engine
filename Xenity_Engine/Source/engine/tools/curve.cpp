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

    if (parentTransform && nextTransform && beforeTransform && parent2Transform) 
    {
        Vector3 parentPos = parentTransform->GetPosition();
        Vector3 nextTransformPos = nextTransform->GetPosition();
        Vector3 beforeTransformPos = beforeTransform->GetPosition();
        Vector3 parent2TransformPos = parent2Transform->GetPosition();

        float pow1 = powf(1 - tVal, 3);
        float pow2 = powf((1 - tVal), 2);
        float pow3 = powf(tVal, 2);
        float pow4 = powf(tVal, 3);

        result.x = pow1 * parentPos.x + 3 * pow2 * tVal * nextTransformPos.x + 3 * (1 - tVal) * pow3 * beforeTransformPos.x + pow4 * parent2TransformPos.x;
        result.y = pow1 * parentPos.y + 3 * pow2 * tVal * nextTransformPos.y + 3 * (1 - tVal) * pow3 * beforeTransformPos.y + pow4 * parent2TransformPos.y;
        result.z = pow1 * parentPos.z + 3 * pow2 * tVal * nextTransformPos.z + 3 * (1 - tVal) * pow3 * beforeTransformPos.z + pow4 * parent2TransformPos.z;
    }

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