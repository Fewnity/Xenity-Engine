// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/game_elements/gameobject.h>
#include <engine/test_component.h>
#include <engine/reflection/reflection_utils.h>

TestResult ReflectiveToJsonToReflectiveTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	TestComponent testComponentA;
	TestComponent testComponentB;

	testComponentA.myBool = true;
	testComponentA.myInt = 5;
	testComponentA.myFloat = 5.5f;
	testComponentA.myString = "Hello World";
	testComponentA.myDouble = 5.5;
	testComponentA.myEnum = Colors::Blue;
	testComponentA.quaternion = Quaternion(1, 0.5f, 0.3f, 0.2f);
	testComponentA.vec2 = Vector2(1, 2);
	testComponentA.vec2Int = Vector2Int(1, 2);
	testComponentA.vec3 = Vector3(1, 2, 3);
	testComponentA.vec4 = Vector4(1, 2, 3, 4);
	testComponentA.myCustomClass.myCustomFloat = 5.5f;
	testComponentA.myCustomClass.myCustomFloat2 = 5.5f;
	testComponentA.myInts.push_back(1);
	testComponentA.myInts.push_back(2);
	testComponentA.myInts.push_back(3);
	testComponentA.myFloats.push_back(1.1f);
	testComponentA.myFloats.push_back(2.2f);
	testComponentA.myFloats.push_back(3.3f);
	testComponentA.myStrings.push_back("Hello");
	testComponentA.myStrings.push_back("World");
	testComponentA.myEnums.push_back(Colors::Blue);
	testComponentA.myEnums.push_back(Colors::Red);

	nlohmann::ordered_json json;
	json["Values"] = ReflectionUtils::ReflectiveToJson(testComponentA);
	ReflectionUtils::JsonToReflective(json, testComponentB);

	EXPECT_EQUALS(testComponentA.myBool, testComponentB.myBool, "myBool is different");
	EXPECT_EQUALS(testComponentA.myInt, testComponentB.myInt, "myInt is different");
	EXPECT_EQUALS(testComponentA.myFloat, testComponentB.myFloat, "myFloat is different");
	EXPECT_EQUALS(testComponentA.myString, testComponentB.myString, "myString is different");
	EXPECT_EQUALS(testComponentA.myDouble, testComponentB.myDouble, "myDouble is different");
	EXPECT_EQUALS(testComponentA.myEnum, testComponentB.myEnum, "myEnum is different");
	EXPECT_EQUALS(testComponentA.quaternion, testComponentB.quaternion, "quaternion is different");
	EXPECT_EQUALS(testComponentA.vec2, testComponentB.vec2, ", is different");
	EXPECT_EQUALS(testComponentA.vec2Int, testComponentB.vec2Int, "vec2Int is different");
	EXPECT_EQUALS(testComponentA.vec3, testComponentB.vec3, "vec3 is different");
	EXPECT_EQUALS(testComponentA.vec4, testComponentB.vec4, "vec4 is different");
	EXPECT_EQUALS(testComponentA.myCustomClass.myCustomFloat, testComponentB.myCustomClass.myCustomFloat, "myInt is different");
	EXPECT_EQUALS(testComponentA.myCustomClass.myCustomFloat2, testComponentB.myCustomClass.myCustomFloat2, "myInt is different");
	EXPECT_EQUALS(testComponentA.myInts, testComponentB.myInts, "myInts is different");
	EXPECT_EQUALS(testComponentA.myFloats, testComponentB.myFloats, "myFloats is different");
	EXPECT_EQUALS(testComponentA.myStrings, testComponentB.myStrings, "myStrings is different");
	EXPECT_EQUALS(testComponentA.myEnums, testComponentB.myEnums, "myEnums is different");

	END_TEST();
}