#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>

UnitTest::UnitTest(const std::string& name)
{
	this->name = name;
}

void UnitTestManager::StartAllTests()
{
	Debug::Print("------ Unit Tests ------");
	VectorAddTest addVectorTest = VectorAddTest("Vectors Additions");
	TryTest(addVectorTest);

	VectorMinusTest minusVectorTest = VectorMinusTest("Vectors Subtractions");
	TryTest(minusVectorTest);

	VectorMultiplyTest multiplyVectorTest = VectorMultiplyTest("Vectors Multiplications");
	TryTest(multiplyVectorTest);

	VectorDivideTest divideVectorTest = VectorDivideTest("Vectors Divisions");
	TryTest(divideVectorTest);

	VectorNormaliseTest normaliseVectorTest = VectorNormaliseTest("Vectors Normalisations");
	TryTest(normaliseVectorTest);

	TransformSetPositionTest transformSetPositionTest = TransformSetPositionTest("Transform Set Position");
	TryTest(transformSetPositionTest);

	TransformSetRotationTest transformSetRotationTest = TransformSetRotationTest("Transform Set Rotation");
	TryTest(transformSetRotationTest);

	TransformSetScaleTest transformSetScaleTest = TransformSetScaleTest("Transform Set Scale");
	TryTest(transformSetScaleTest);

	Debug::Print("------ Unit Tests finished! ------");
}

void UnitTestManager::TryTest(UnitTest& test)
{
	std::string errorOut = "";
	bool testResult = test.Start(errorOut);
	if (testResult)
	{
		Debug::Print(test.GetName() + " Passed");
	}
	else
	{
		Debug::PrintError(test.GetName() + ": " + errorOut);
	}
}