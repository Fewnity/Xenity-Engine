// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>

TestResult VectorAddTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// Test Vector2
	const Vector2 v2A = Vector2(1, 4.5f);
	const Vector2 v2B = Vector2(3, 1);
	EXPECT_EQUALS(v2A + v2B, Vector2(4, 5.5f), "Bad Vector2 addition");

	// Test Vector2Int
	const Vector2Int v2IntA = Vector2Int(1, 4);
	const Vector2Int v2IntB = Vector2Int(3, 1);
	EXPECT_EQUALS(v2IntA + v2IntB, Vector2Int(4, 5), "Bad Vector2Int addition");

	// Test Vector3
	const Vector3 v3A = Vector3(1, 4.5f, 6);
	const Vector3 v3B = Vector3(3, 1, -2);
	EXPECT_EQUALS(v3A + v3B, Vector3(4, 5.5f, 4), "Bad Vector3 addition");

	// Test Vector4
	const Vector4 v4A = Vector4(1, 4.5f, 6, 5);
	const Vector4 v4B = Vector4(3, 1, -2, 5);
	EXPECT_EQUALS(v4A + v4B, Vector4(4, 5.5f, 4, 10), "Bad Vector4 addition");

	END_TEST();
}

TestResult VectorMinusTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// Test Vector2
	const Vector2 v2A = Vector2(1, 4.5f);
	const Vector2 v2B = Vector2(3, 1);
	EXPECT_EQUALS(v2A - v2B, Vector2(-2, 3.5f), "Bad Vector2 subtraction");

	// Test Vector2Int
	const Vector2Int v2IntA = Vector2Int(1, 4);
	const Vector2Int v2IntB = Vector2Int(3, 1);
	EXPECT_EQUALS(v2IntA - v2IntB, Vector2Int(-2, 3), "Bad Vector2Int subtraction");

	// Test Vector3
	const Vector3 v3A = Vector3(1, 4.5f, 6);
	const Vector3 v3B = Vector3(3, 1, -2);
	EXPECT_EQUALS(v3A - v3B, Vector3(-2, 3.5f, 8), "Bad Vector3 subtraction");

	// Test Vector4
	const Vector4 v4A = Vector4(1, 4.5f, 6, 5);
	const Vector4 v4B = Vector4(3, 1, -2, 5);
	EXPECT_EQUALS(v4A - v4B, Vector4(-2, 3.5f, 8, 0), "Bad Vector4 subtraction");

	END_TEST();
}

TestResult VectorMultiplyTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// Test Vector2
	const Vector2 v2A = Vector2(2, 4.5f);
	const Vector2 v2B = Vector2(3, 2);
	EXPECT_EQUALS(v2A * v2B, Vector2(6, 9), "Bad Vector2 multiplication");

	// Test Vector2Int
	const Vector2Int v2IntA = Vector2Int(2, 4);
	const Vector2Int v2IntB = Vector2Int(3, 2);
	EXPECT_EQUALS(v2IntA * v2IntB, Vector2Int(6, 8), "Bad Vector2Int multiplication");

	// Test Vector3
	const Vector3 v3A = Vector3(2, 4.5f, 6);
	const Vector3 v3B = Vector3(3, 2, -2);
	EXPECT_EQUALS(v3A * v3B, Vector3(6, 9, -12), "Bad Vector3 multiplication");

	// Test Vector4
	const Vector4 v4A = Vector4(2, 4.5f, 6, 5);
	const Vector4 v4B = Vector4(3, 2, -2, 5);
	EXPECT_EQUALS(v4A * v4B, Vector4(6, 9, -12, 25), "Bad Vector4 multiplication");

	END_TEST();
}

TestResult VectorDivideTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// Test Vector2
	const Vector2 v2A = Vector2(6, 3);
	const Vector2 v2B = Vector2(2, 2);
	EXPECT_EQUALS(v2A / v2B, Vector2(3, 1.5f), "Bad Vector2 division");

	// Test Vector2Int
	const Vector2Int v2IntA = Vector2Int(6, 3);
	const Vector2Int v2IntB = Vector2Int(2, 2);
	EXPECT_EQUALS(v2IntA / v2IntB, Vector2Int(3, 1), "Bad Vector2Int division");

	// Test Vector3
	const Vector3 v3A = Vector3(6, 3, 6);
	const Vector3 v3B = Vector3(2, 2, 3);
	EXPECT_EQUALS(v3A / v3B, Vector3(3, 1.5f, 2), "Bad Vector3 division");

	// Test Vector4
	const Vector4 v4A = Vector4(6, 3, 6, 0);
	const Vector4 v4B = Vector4(2, 2, 3, 5);
	EXPECT_EQUALS(v4A / v4B, Vector4(3, 1.5f, 2, 0), "Bad Vector4 division");

	END_TEST();
}

TestResult VectorNormalizeTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// Test Vector2
	const Vector2 v2A = Vector2(6, 3);
	EXPECT_EQUALS(v2A.Normalized(), Vector2(0.894427180f, 0.447213590f), "Bad Vector2 normalization");

	// Test Vector3
	const Vector3 v3A = Vector3(6, 3, 9);
	EXPECT_EQUALS(v3A.Normalized(), Vector3(0.534522474f, 0.267261237f, 0.801783741f), "Bad Vector3 normalization");

	// Test Vector4
	const Vector4 v4A = Vector4(6, 3, 9, -1);
	EXPECT_EQUALS(v4A.Normalized(), Vector4(0.532413900f, 0.266206950f, 0.798620880f, -0.0887356550f), "Bad Vector4 normalization");

	END_TEST();
}