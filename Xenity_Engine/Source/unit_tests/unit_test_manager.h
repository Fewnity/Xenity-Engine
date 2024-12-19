// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>

class GameObject;

class UnitTest 
{
public:
	UnitTest() = delete;
	UnitTest(const std::string& _name);

	/**
	* @brief Start the test and return true if the test is successful
	*/
	virtual bool Start(std::string& errorOut) = 0;

	/**
	* @brief Compare two values of the same type and return true if they are equal
	*/
	template <typename T>
	bool Compare(const T& valueA, const T& valueB) const
	{
		return valueA == valueB;
	}

	/**
	* @brief Compare two values of different types and return true if they are equal example compare float and int
	*/
	template <typename T, typename U>
	bool Compare(const T& valueA, const U& valueB) const
	{
		return valueA == valueB;
	}

	/**
	* @brief Get the name of the test
	*/
	const std::string& GetName() const
	{
		return name;
	}

private:
	std::string name;
};

#define EXPECT_EQUALS(a, b, errorText) if (!Compare(a, b)) { result = false; errorOut += std::string(errorText) + "\n"; }
#define EXPECT_NOT_EQUALS(a, b, errorText) if (Compare(a, b)) { result = false; errorOut += std::string(errorText) + "\n"; }

#define EXPECT_NULL(a, errorText) if (a) { result = false; errorOut += std::string(errorText) + "\n"; }
#define EXPECT_NOT_NULL(a, errorText) if (!a) { result = false; errorOut += std::string(errorText) + "\n"; }

#define EXPECT_TRUE(a, errorText) if (a != true) { result = false; errorOut += std::string(errorText) + "\n"; }
#define EXPECT_FALSE(a, errorText) if (a != false) { result = false; errorOut += std::string(errorText) + "\n"; }

#define EXPECT_NEAR(value, expectedValue, errorText) if (value > expectedValue + std::numeric_limits<float>::epsilon() || value < expectedValue - std::numeric_limits<float>::epsilon()) { result = false; errorOut += std::string(errorText) + "\n"; }

class UnitTestManager
{
public:
	static void StartAllTests();
	static void TryTest(UnitTest& RegisterEnumStringsMap);
};

using TestResult = bool;

#define BEGIN_TEST() TestResult result = true
#define MAKE_TEST(testName) class testName##Test : public UnitTest { public: testName##Test(const std::string& name) : UnitTest(name) { } bool Start(std::string& errorOut) override; }
#define END_TEST() return result

// Missing tests:
// ----- Engine -----
// - Quaternion
// - GameObject
// - Component
// - Physics
// - Reflection To Json then back to reflection object
// ----- Editor -----
// - Delete commands
// - Modify commands
// - FileReferenceFinder

#pragma region Vector

// Incomplete tests!

MAKE_TEST(VectorAdd);
MAKE_TEST(VectorMinus);
MAKE_TEST(VectorMultiply);
MAKE_TEST(VectorDivide);
MAKE_TEST(VectorNormalize);

#pragma endregion

#pragma region Transform

// Incomplete tests!

MAKE_TEST(TransformSetPosition);
MAKE_TEST(TransformSetRotation);
MAKE_TEST(TransformSetScale);

#pragma endregion

#pragma region Color

// Need an update!

MAKE_TEST(ColorConstructor);
MAKE_TEST(ColorSet);

#pragma endregion

#pragma region Event System

class EventSystemTest : public UnitTest
{
public:
	EventSystemTest(const std::string& name) : UnitTest(name) { }

	static void EventFunction(int& value);
	void EventObjectFunction(int& value);

	bool Start(std::string& errorOut) override;
};

#pragma endregion

#pragma region Math

MAKE_TEST(MathBasic);
MAKE_TEST(MathMatrix);

#pragma endregion

#pragma region Asset Manager

MAKE_TEST(AssetManager);

#pragma endregion


#pragma region Class Registry

class ClassRegistryAddComponentFromNameTest : public UnitTest
{
public:
	ClassRegistryAddComponentFromNameTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;

	template <typename T>
	void TestAddComponent(std::shared_ptr<GameObject>& newGameObject, bool& result, std::string& errorOut, const std::string& componentName);
};

MAKE_TEST(ClassRegistryGetComponentNames);

#pragma endregion

#pragma region Unique Id

MAKE_TEST(UniqueId);

#pragma endregion

#pragma region Benchmark

MAKE_TEST(Benchmark);

#pragma endregion

#pragma region Endian

MAKE_TEST(EndianCheck);
MAKE_TEST(EndianSwap);

#pragma endregion

#pragma region Reflection

MAKE_TEST(ReflectiveToJsonToReflective);

#pragma endregion

// ------------------------------------------------------------------------------- EDITOR TESTS

#pragma region Editor

#pragma region Create Command

// Incomplete tests!

MAKE_TEST(AddComponentCommand);

#pragma endregion

#pragma endregion // Editor