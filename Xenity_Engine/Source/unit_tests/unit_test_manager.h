// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

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

//
// --------------------- Vector
//

class VectorAddTest : public UnitTest
{
public:
	VectorAddTest() = delete;
	VectorAddTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorMinusTest : public UnitTest
{
public:
	VectorMinusTest() = delete;
	VectorMinusTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorMultiplyTest : public UnitTest
{
public:
	VectorMultiplyTest() = delete;
	VectorMultiplyTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorDivideTest : public UnitTest
{
public:
	VectorDivideTest() = delete;
	VectorDivideTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class VectorNormalizeTest : public UnitTest
{
public:
	VectorNormalizeTest() = delete;
	VectorNormalizeTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

//
// --------------------- Transform
//

class TransformSetPositionTest : public UnitTest
{
public:
	TransformSetPositionTest() = delete;
	TransformSetPositionTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class TransformSetRotationTest : public UnitTest
{
public:
	TransformSetRotationTest() = delete;
	TransformSetRotationTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class TransformSetScaleTest : public UnitTest
{
public:
	TransformSetScaleTest() = delete;
	TransformSetScaleTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

//
// --------------------- Color
//

class ColorConstructorTest : public UnitTest
{
public:
	ColorConstructorTest() = delete;
	ColorConstructorTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class ColorSetTest : public UnitTest
{
public:
	ColorSetTest() = delete;
	ColorSetTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

//
// --------------------- Event System
//

class EventSystemTest : public UnitTest
{
public:
	EventSystemTest() = delete;
	EventSystemTest(const std::string& name) : UnitTest(name) { }

	static void EventFunction(int& value);
	void EventObjectFunction(int& value);

	bool Start(std::string& errorOut) override;
};

//
// --------------------- Math
//

class MathBasicTest : public UnitTest
{
public:
	MathBasicTest() = delete;
	MathBasicTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};

class MathMatrixTest : public UnitTest
{
public:
	MathMatrixTest() = delete;
	MathMatrixTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut) override;
};