#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>

bool VectorAddTest::Start(std::string& errorOut)
{
	bool result = true;

	// Test Vector2
	Vector2 v2A = Vector2(1, 4.5f);
	Vector2 v2B = Vector2(3, 1);
	if (!Compare(v2A + v2B, Vector2(4, 5.5f)))
	{
		errorOut += "Bad Vector2 addition\n";
		result = false;
	}

	// Test Vector2Int
	Vector2Int v2IntA = Vector2Int(1, 4);
	Vector2Int v2IntB = Vector2Int(3, 1);
	if (!Compare(v2IntA + v2IntB, Vector2Int(4, 5)))
	{
		errorOut += "Bad Vector2Int addition\n";
		result = false;
	}

	// Test Vector3
	Vector3 v3A = Vector3(1, 4.5f, 6);
	Vector3 v3B = Vector3(3, 1, -2);
	if (!Compare(v3A + v3B, Vector3(4, 5.5f, 4)))
	{
		errorOut += "Bad Vector3 addition\n";
		result = false;
	}

	// Test Vector4
	Vector4 v4A = Vector4(1, 4.5f, 6, 5);
	Vector4 v4B = Vector4(3, 1, -2, 5);
	if (!Compare(v4A + v4B, Vector4(4, 5.5f, 4, 10)))
	{
		errorOut += "Bad Vector4 addition\n";
		result = false;
	}

	return result;
}

bool VectorMinusTest::Start(std::string& errorOut)
{
	bool result = true;

	// Test Vector2
	Vector2 v2A = Vector2(1, 4.5f);
	Vector2 v2B = Vector2(3, 1);
	if (!Compare(v2A - v2B, Vector2(-2, 3.5f)))
	{
		errorOut += "Bad Vector2 subtraction\n";
		result = false;
	}

	// Test Vector2Int
	Vector2Int v2IntA = Vector2Int(1, 4);
	Vector2Int v2IntB = Vector2Int(3, 1);
	if (!Compare(v2IntA - v2IntB, Vector2Int(-2, 3)))
	{
		errorOut += "Bad Vector2Int subtraction\n";
		result = false;
	}

	// Test Vector3
	Vector3 v3A = Vector3(1, 4.5f, 6);
	Vector3 v3B = Vector3(3, 1, -2);
	if (!Compare(v3A - v3B, Vector3(-2, 3.5f, 8)))
	{
		errorOut += "Bad Vector3 subtraction\n";
		result = false;
	}

	// Test Vector4
	Vector4 v4A = Vector4(1, 4.5f, 6, 5);
	Vector4 v4B = Vector4(3, 1, -2, 5);
	if (!Compare(v4A - v4B, Vector4(-2, 3.5f, 8, 0)))
	{
		errorOut += "Bad Vector4 subtraction\n";
		result = false;
	}

	return result;
}

bool VectorMultiplyTest::Start(std::string& errorOut)
{
	bool result = true;

	// Test Vector2
	Vector2 v2A = Vector2(2, 4.5f);
	Vector2 v2B = Vector2(3, 2);
	if (!Compare(v2A * v2B, Vector2(6, 9)))
	{
		errorOut += "Bad Vector2 multiplication\n";
		result = false;
	}

	// Test Vector2Int
	Vector2Int v2IntA = Vector2Int(2, 4);
	Vector2Int v2IntB = Vector2Int(3, 2);
	if (!Compare(v2IntA * v2IntB, Vector2Int(6, 8)))
	{
		errorOut += "Bad Vector2Int multiplication\n";
		result = false;
	}

	// Test Vector3
	Vector3 v3A = Vector3(2, 4.5f, 6);
	Vector3 v3B = Vector3(3, 2, -2);
	if (!Compare(v3A * v3B, Vector3(6, 9, -12)))
	{
		errorOut += "Bad Vector3 multiplication\n";
		result = false;
	}

	// Test Vector4
	Vector4 v4A = Vector4(2, 4.5f, 6, 5);
	Vector4 v4B = Vector4(3, 2, -2, 5);
	if (!Compare(v4A * v4B, Vector4(6, 9, -12, 25)))
	{
		errorOut += "Bad Vector4 multiplication\n";
		result = false;
	}

	return result;
}

bool VectorDivideTest::Start(std::string& errorOut)
{
	bool result = true;

	// Test Vector2
	Vector2 v2A = Vector2(6, 3);
	Vector2 v2B = Vector2(2, 2);
	if (!Compare(v2A / v2B, Vector2(3, 1.5f)))
	{
		errorOut += "Bad Vector2 division\n";
		result = false;
	}

	// Test Vector2Int
	Vector2Int v2IntA = Vector2Int(6, 3);
	Vector2Int v2IntB = Vector2Int(2, 2);
	if (!Compare(v2IntA / v2IntB, Vector2Int(3, 1)))
	{
		errorOut += "Bad Vector2Int division\n";
		result = false;
	}

	// Test Vector3
	Vector3 v3A = Vector3(6, 3, 6);
	Vector3 v3B = Vector3(2, 2, 3);
	if (!Compare(v3A / v3B, Vector3(3, 1.5f, 2)))
	{
		errorOut += "Bad Vector3 division\n";
		result = false;
	}

	// Test Vector4
	Vector4 v4A = Vector4(6, 3, 6, 0);
	Vector4 v4B = Vector4(2, 2, 3, 5);
	if (!Compare(v4A / v4B, Vector4(3, 1.5f, 2, 0)))
	{
		errorOut += "Bad Vector4 division\n";
		result = false;
	}

	return result;
}

bool VectorNormalizeTest::Start(std::string& errorOut)
{
	bool result = true;

	// Test Vector2
	Vector2 v2A = Vector2(6, 3);
	if (!Compare(v2A.Normalized(), Vector2(0.894427180f, 0.447213590f)))
	{
		errorOut += "Bad Vector2 normalization\n";
		result = false;
	}

	// Test Vector3
	Vector3 v3A = Vector3(6, 3, 9);
	if (!Compare(v3A.Normalized(), Vector3(0.534522474f, 0.267261237f, 0.801783741f)))
	{
		errorOut += "Bad Vector3 normalization\n";
		result = false;
	}

	// Test Vector4
	Vector4 v4A = Vector4(6, 3, 9, -1);
	if (!Compare(v4A.Normalized(), Vector4(0.532413900f, 0.266206950f, 0.798620880f, -0.0887356550f)))
	{
		errorOut += "Bad Vector4 normalization\n";
		result = false;
	}

	return result;
}