#pragma once

#include <string>

class UnitTest {
public:
	UnitTest() = delete;
	UnitTest(const std::string& name);
	virtual bool Start(std::string& errorOut) = 0;

	template <typename T>
	bool Compare(T valueA, T valueB)
	{
		return valueA == valueB;
	}

	std::string GetName() 
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
	static void TryTest(UnitTest* test);
};

class AddVectorTest : public UnitTest
{
public:
	AddVectorTest() = delete;
	AddVectorTest(const std::string& name) : UnitTest(name){ }

	bool Start(std::string& errorOut);
};

class MinusVectorTest : public UnitTest
{
public:
	MinusVectorTest() = delete;
	MinusVectorTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut);
};

class MultiplyVectorTest : public UnitTest
{
public:
	MultiplyVectorTest() = delete;
	MultiplyVectorTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut);
};

class DivideVectorTest : public UnitTest
{
public:
	DivideVectorTest() = delete;
	DivideVectorTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut);
};

class NormaliseVectorTest : public UnitTest
{
public:
	NormaliseVectorTest() = delete;
	NormaliseVectorTest(const std::string& name) : UnitTest(name) { }

	bool Start(std::string& errorOut);
};