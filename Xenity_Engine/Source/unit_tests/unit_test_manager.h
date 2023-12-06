#pragma once

#include <string>

class UnitTest 
{
public:
	UnitTest() = delete;
	UnitTest(const std::string& name);
	virtual bool Start(std::string& errorOut) = 0;

	template <typename T>
	bool Compare(const T& valueA, const T& valueB) const
	{
		return valueA == valueB;
	}

	std::string GetName() const
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
	static void TryTest(UnitTest& test);
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

class VectorNormaliseTest : public UnitTest
{
public:
	VectorNormaliseTest() = delete;
	VectorNormaliseTest(const std::string& name) : UnitTest(name) { }

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