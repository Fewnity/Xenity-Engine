// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "curve.h"

#include <engine/engine.h>

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>


Vector3 Spline::GetValueAt(const float t) const
{
	const int curveCount = (int)splinePoints.size() - 1;
	int currentCurve = (int)floorf(t * curveCount);
	if (currentCurve == curveCount)
		currentCurve--;

	float tVal = t * curveCount;
	tVal -= currentCurve;

	Vector3 result = Vector3();

	const std::shared_ptr<Transform> parentTransform = splinePoints[0 + currentCurve]->parent.lock();
	const std::shared_ptr<Transform> nextTransform = splinePoints[0 + currentCurve]->next.lock();
	const std::shared_ptr<Transform> beforeTransform = splinePoints[1 + currentCurve]->before.lock();
	const std::shared_ptr<Transform> parent2Transform = splinePoints[1 + currentCurve]->parent.lock();

	if (parentTransform && nextTransform && beforeTransform && parent2Transform)
	{
		const Vector3& parentPos = parentTransform->GetPosition();
		const Vector3& nextTransformPos = nextTransform->GetPosition();
		const Vector3& beforeTransformPos = beforeTransform->GetPosition();
		const Vector3& parent2TransformPos = parent2Transform->GetPosition();

		const float pow1 = powf(1 - tVal, 3);
		const float pow2 = powf((1 - tVal), 2);
		const float pow3 = powf(tVal, 2);
		const float pow4 = powf(tVal, 3);

		result.x = pow1 * parentPos.x + 3 * pow2 * tVal * nextTransformPos.x + 3 * (1 - tVal) * pow3 * beforeTransformPos.x + pow4 * parent2TransformPos.x;
		result.y = pow1 * parentPos.y + 3 * pow2 * tVal * nextTransformPos.y + 3 * (1 - tVal) * pow3 * beforeTransformPos.y + pow4 * parent2TransformPos.y;
		result.z = pow1 * parentPos.z + 3 * pow2 * tVal * nextTransformPos.z + 3 * (1 - tVal) * pow3 * beforeTransformPos.z + pow4 * parent2TransformPos.z;
	}

	return result;
}

SplinePoint* Spline::CreateSplinePoint(const Vector3& position)
{
	SplinePoint* point = new SplinePoint();

	std::shared_ptr<GameObject> parent = CreateGameObject();
	parent->GetTransform()->SetPosition(position);

	std::shared_ptr<GameObject> next = CreateGameObject();
	parent->AddChild(next);
	next->GetTransform()->SetLocalPosition(Vector3(0.5f, 0, 0));

	std::shared_ptr<GameObject> before = CreateGameObject();
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