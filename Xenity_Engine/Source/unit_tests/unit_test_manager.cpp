#include "unit_test_manager.h"
#include "../xenity.h"

UnitTest::UnitTest(const std::string& name)
{
	this->name = name;
}

void UnitTestManager::StartAllTests()
{
	Debug::Print("------ Unit Tests ------");
	AddVectorTest addVectorTest = AddVectorTest("Vectors Additions");
	TryTest(&addVectorTest);

	MinusVectorTest minusVectorTest = MinusVectorTest("Vectors Subtractions");
	TryTest(&minusVectorTest);

	MultiplyVectorTest multiplyVectorTest = MultiplyVectorTest("Vectors Multiplications");
	TryTest(&multiplyVectorTest);

	DivideVectorTest divideVectorTest = DivideVectorTest("Vectors Divisions");
	TryTest(&divideVectorTest);

	NormaliseVectorTest normaliseVectorTest = NormaliseVectorTest("Vectors Normalisations");
	TryTest(&normaliseVectorTest);

	Debug::Print("------ Unit Tests finished! ------");
}

void UnitTestManager::TryTest(UnitTest* test) 
{
	std::string errorOut = "";
	bool testResult = test->Start(errorOut);
	if (testResult)
	{
		Debug::Print(test->GetName() + " Passed");
	}
	else
	{
		Debug::PrintError(test->GetName() + ": " + errorOut);
	}
}

bool AddVectorTest::Start(std::string& errorOut)
{
	//Test Vector2
	Vector2 v2A = Vector2(1, 4.5f);
	Vector2 v2B = Vector2(3, 1);

	if (!Compare(v2A + v2B, Vector2(4, 5.5f)))
	{
		errorOut = "Bad Vector2 addition";
		return false;
	}

	//Test Vector2Int
	Vector2Int v2IntA = Vector2Int(1, 4);
	Vector2Int v2IntB = Vector2Int(3, 1);

	if (!Compare(v2IntA + v2IntB, Vector2Int(4, 5)))
	{
		errorOut = "Bad Vector2Int addition";
		return false;
	}

	//Test Vector3
	Vector3 v3a = Vector3(1,4.5f,6);
	Vector3 v3b = Vector3(3, 1, -2);

	if (!Compare(v3a + v3b, Vector3(4,5.5f,4)))
	{
		errorOut = "Bad Vector3 addition";
		return false;
	}

	//Test Vector4
	Vector4 v4a = Vector4(1, 4.5f, 6, 5);
	Vector4 v4b = Vector4(3, 1, -2, 5);

	if (!Compare(v4a + v4b, Vector4(4, 5.5f, 4, 10)))
	{
		errorOut = "Bad Vector4 addition";
		return false;
	}

	return true;
}

bool MinusVectorTest::Start(std::string& errorOut)
{
	//Test Vector2
	Vector2 v2A = Vector2(1, 4.5f);
	Vector2 v2B = Vector2(3, 1);

	if (!Compare(v2A - v2B, Vector2(-2, 3.5f)))
	{
		errorOut = "Bad Vector2 subtraction";
		return false;
	}

	//Test Vector2Int
	Vector2Int v2IntA = Vector2Int(1, 4);
	Vector2Int v2IntB = Vector2Int(3, 1);

	if (!Compare(v2IntA - v2IntB, Vector2Int(-2, 3)))
	{
		errorOut = "Bad Vector2Int subtraction";
		return false;
	}

	//Test Vector3
	Vector3 v3a = Vector3(1, 4.5f, 6);
	Vector3 v3b = Vector3(3, 1, -2);

	if (!Compare(v3a - v3b, Vector3(-2, 3.5f, 8)))
	{
		errorOut = "Bad Vector3 subtraction";
		return false;
	}

	//Test Vector4
	Vector4 v4a = Vector4(1, 4.5f, 6, 5);
	Vector4 v4b = Vector4(3, 1, -2, 5);

	if (!Compare(v4a - v4b, Vector4(-2, 3.5f, 8, 0)))
	{
		errorOut = "Bad Vector4 subtraction";
		return false;
	}

	return true;
}

bool MultiplyVectorTest::Start(std::string& errorOut)
{
	//Test Vector2
	Vector2 v2A = Vector2(2, 4.5f);
	Vector2 v2B = Vector2(3, 2);

	if (!Compare(v2A * v2B, Vector2(6, 9)))
	{
		errorOut = "Bad Vector2 multiplication";
		return false;
	}

	//Test Vector2Int
	Vector2Int v2IntA = Vector2Int(2, 4);
	Vector2Int v2IntB = Vector2Int(3, 2);

	if (!Compare(v2IntA * v2IntB, Vector2Int(6, 8)))
	{
		errorOut = "Bad Vector2Int multiplication";
		return false;
	}

	//Test Vector3
	Vector3 v3a = Vector3(2, 4.5f, 6);
	Vector3 v3b = Vector3(3, 2, -2);

	if (!Compare(v3a * v3b, Vector3(6, 9, -12)))
	{
		errorOut = "Bad Vector3 multiplication";
		return false;
	}

	//Test Vector4
	Vector4 v4a = Vector4(2, 4.5f, 6, 5);
	Vector4 v4b = Vector4(3, 2, -2, 5);

	if (!Compare(v4a * v4b, Vector4(6, 9, -12, 25)))
	{
		errorOut = "Bad Vector4 multiplication";
		return false;
	}

	return true;
}

bool DivideVectorTest::Start(std::string& errorOut)
{
	//Test Vector2
	Vector2 v2A = Vector2(6, 3);
	Vector2 v2B = Vector2(2, 2);

	if (!Compare(v2A / v2B, Vector2(3, 1.5f)))
	{
		errorOut = "Bad Vector2 division";
		return false;
	}

	//Test Vector2Int
	Vector2Int v2IntA = Vector2Int(6, 3);
	Vector2Int v2IntB = Vector2Int(2, 2);

	if (!Compare(v2IntA / v2IntB, Vector2Int(3, 1)))
	{
		errorOut = "Bad Vector2Int division";
		return false;
	}

	//Test Vector3
	Vector3 v3a = Vector3(6,3, 6);
	Vector3 v3b = Vector3(2, 2, 3);

	if (!Compare(v3a / v3b, Vector3(3, 1.5f, 2)))
	{
		errorOut = "Bad Vector3 division";
		return false;
	}

	//Test Vector4
	Vector4 v4a = Vector4(6, 3, 6, 0);
	Vector4 v4b = Vector4(2, 2, 3, 5);

	if (!Compare(v4a / v4b, Vector4(3, 1.5f,2, 0)))
	{
		errorOut = "Bad Vector4 division";
		return false;
	}

	return true;
}

bool NormaliseVectorTest::Start(std::string& errorOut)
{
	//Test Vector2
	Vector2 v2A = Vector2(6, 3);
	if (!Compare(v2A.Normalize(), Vector2(0.894427180f, 0.447213590f)))
	{
		errorOut = "Bad Vector2 normalisation";
		return false;
	}

	//Test Vector3
	Vector3 v3a = Vector3(6, 3, 9);
	if (!Compare(v3a.Normalise(), Vector3(0.534522474f, 0.267261237f, 0.801783741f)))
	{
		errorOut = "Bad Vector3 normalisation";
		return false;
	}

	//Test Vector4
	Vector4 v4a = Vector4(6, 3, 9, -1);
	if (!Compare(v4a.Normalise(), Vector4(0.532413900f, 0.266206950f, 0.798620880f, -0.0887356550f)))
	{
		errorOut = "Bad Vector4 normalisation";
	return false;
	}

	return true;
}