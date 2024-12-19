// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/tools/math.h>
#include <engine/vectors/vector3.h>
#include <glm/mat4x4.hpp>

TestResult MathBasicTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// ----------------- Clamp test
	const float clampValue0 = Math::Clamp(5.0f, -1.0f, 10.0f);
	const float clampValue1 = Math::Clamp(-2.0f, -1.0f, 10.0f);
	const float clampValue2 = Math::Clamp(15.0f, -1.0f, 10.0f);

	EXPECT_NEAR(clampValue0, 5.0f, "Bad Math Clamp");
	EXPECT_NEAR(clampValue1, -1.0f, "Bad Math Clamp");
	EXPECT_NEAR(clampValue2, 10.0f, "Bad Math Clamp");

	// ----------------- Lerp test
	const float lerpValue0 = Math::Lerp(1.0f, 2.0f, 0.5f);
	const float lerpValue1 = Math::Lerp(1.0f, 2.0f, 1.0f);
	const float lerpValue2 = Math::Lerp(1.0f, 2.0f, 0.0f);

	EXPECT_NEAR(lerpValue0, 1.5f, "Bad Math Lerp");
	EXPECT_NEAR(lerpValue1, 2.0f, "Bad Math Lerp");
	EXPECT_NEAR(lerpValue2, 1.0f, "Bad Math Lerp");

	const int nextPow2_0 = Math::nextPow2(256);
	const int nextPow2_1 = Math::nextPow2(255);
	const int nextPow2_2 = Math::nextPow2(257);

	EXPECT_NEAR(nextPow2_0, 256, "Bad Math NextPow2");
	EXPECT_NEAR(nextPow2_1, 256, "Bad Math NextPow2");
	EXPECT_NEAR(nextPow2_2, 512, "Bad Math NextPow2");

	const int previousPow2_0 = Math::previousPow2(256);
	const int previousPow2_1 = Math::previousPow2(255);
	const int previousPow2_2 = Math::previousPow2(257);

	EXPECT_NEAR(previousPow2_0, 256, "Bad Math PreviousPow2");
	EXPECT_NEAR(previousPow2_1, 128, "Bad Math PreviousPow2");
	EXPECT_NEAR(previousPow2_2, 256, "Bad Math PreviousPow2");

	END_TEST();
}

TestResult MathMatrixTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	const glm::mat4 simpleMatrix = Math::CreateModelMatrix(Vector3(1.0f, 2.0f, 3.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

	const glm::vec3 position = simpleMatrix[3];
	EXPECT_EQUALS(position, glm::vec3(-1.0f, 2.0f, 3.0f), "Bad Math CreateModelMatrix Position");

	END_TEST();
}