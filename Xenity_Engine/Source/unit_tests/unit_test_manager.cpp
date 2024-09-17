// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>

UnitTest::UnitTest(const std::string& _name) : name(_name)
{
}

void UnitTestManager::StartAllTests()
{
	Debug::Print("------ Unit Tests ------", true);

	//------------------------------------------------------------------ Test vectors
	{
		VectorAddTest addVectorTest = VectorAddTest("Vectors Additions");
		TryTest(addVectorTest);

		VectorMinusTest minusVectorTest = VectorMinusTest("Vectors Subtractions");
		TryTest(minusVectorTest);

		VectorMultiplyTest multiplyVectorTest = VectorMultiplyTest("Vectors Multiplications");
		TryTest(multiplyVectorTest);

		VectorDivideTest divideVectorTest = VectorDivideTest("Vectors Divisions");
		TryTest(divideVectorTest);

		VectorNormalizeTest normalizeVectorTest = VectorNormalizeTest("Vectors Normalizations");
		TryTest(normalizeVectorTest);
	}

	//------------------------------------------------------------------ Test transform
	{
		TransformSetPositionTest transformSetPositionTest = TransformSetPositionTest("Transform Set Position");
		TryTest(transformSetPositionTest);

		TransformSetRotationTest transformSetRotationTest = TransformSetRotationTest("Transform Set Rotation");
		TryTest(transformSetRotationTest);

		TransformSetScaleTest transformSetScaleTest = TransformSetScaleTest("Transform Set Scale");
		TryTest(transformSetScaleTest);
	}

	//------------------------------------------------------------------ Test color
	{
		ColorConstructorTest colorConstructorTest = ColorConstructorTest("Color Constructor");
		TryTest(colorConstructorTest);

		ColorSetTest colorSetTest = ColorSetTest("Color Set");
		TryTest(colorSetTest);
	}

	//------------------------------------------------------------------ Test Event System
	{
		EventSystemTest eventSystemTest = EventSystemTest("Event System");
		TryTest(eventSystemTest);
	}

	//------------------------------------------------------------------ Test Math
	{
		MathBasicTest mathBasicTest = MathBasicTest("Math Basics");
		TryTest(mathBasicTest);

		MathMatrixTest mathMatrixTest = MathMatrixTest("Math Matrice");
		TryTest(mathMatrixTest);
	}

	//------------------------------------------------------------------ Asset Manager
	{
		AssetManagerTest assetManagerTest = AssetManagerTest("Asset Manager");
		TryTest(assetManagerTest);
	}

	//------------------------------------------------------------------ ClassRegistry Test
	{
		ClassRegistryTest classRegistryTest = ClassRegistryTest("Asset Manager");
		TryTest(classRegistryTest);
	}

	Debug::Print("------ Unit Tests finished! ------", true);
}

void UnitTestManager::TryTest(UnitTest& RegisterEnumStringsMap)
{
	std::string errorOut = "";
	const bool testResult = RegisterEnumStringsMap.Start(errorOut);
	if (testResult)
	{
		Debug::Print(RegisterEnumStringsMap.GetName() + " Passed", true);
	}
	else
	{
		Debug::PrintError(RegisterEnumStringsMap.GetName() + ": " + errorOut, true);
	}
}