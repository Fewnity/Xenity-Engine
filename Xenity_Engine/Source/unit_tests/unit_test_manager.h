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
	virtual bool Start(std::string& errorOut) = 0;

	/**
	* @brief Compare two values and return true if they are equal
	*/
	template <typename T>
	bool Compare(const T& valueA, const T& valueB) const
	{
		return valueA == valueB;
	}

	template <typename T, typename U>
	bool Compare(const T& valueA, const U& valueB) const
	{
		return valueA == valueB;
	}

	const std::string& GetName() const
	{
		return name;
	}

private:
	std::string name;
};

class UnitTestManager
{
public:
	static void StartAllTests();
	static void TryTest(UnitTest& RegisterEnumStringsMap);
};

#pragma region Vector

class VectorAddTest : public UnitTest
{
public:
	VectorAddTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorMinusTest : public UnitTest
{
public:
	VectorMinusTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorMultiplyTest : public UnitTest
{
public:
	VectorMultiplyTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorDivideTest : public UnitTest
{
public:
	VectorDivideTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorNormalizeTest : public UnitTest
{
public:
	VectorNormalizeTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

#pragma endregion

#pragma region Transform

class TransformSetPositionTest : public UnitTest
{
public:
	TransformSetPositionTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class TransformSetRotationTest : public UnitTest
{
public:
	TransformSetRotationTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class TransformSetScaleTest : public UnitTest
{
public:
	TransformSetScaleTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

#pragma endregion

#pragma region Color

class ColorConstructorTest : public UnitTest
{
public:
	ColorConstructorTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class ColorSetTest : public UnitTest
{
public:
	ColorSetTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

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

class MathBasicTest : public UnitTest
{
public:
	MathBasicTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class MathMatrixTest : public UnitTest
{
public:
	MathMatrixTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

#pragma endregion

#pragma region Asset Manager

class AssetManagerTest : public UnitTest
{
public:
	AssetManagerTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

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

class ClassRegistryGetComponentNamesTest : public UnitTest
{
public:
	ClassRegistryGetComponentNamesTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

#pragma endregion
