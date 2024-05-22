// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/graphics/color/color.h>

void EventSystemTest::EventFunction(int& value) 
{
	value++;
}

void EventSystemTest::EventObjectFunction(int& value) 
{
	value *= 2;
}

bool EventSystemTest::Start(std::string& errorOut)
{
	bool result = true;
	int eventValue = 0;

	Event<int&> myEvent;

	// ----------------- Constructor test

	if (!Compare(myEvent.GetBindedFunctionCount(), 0))
	{
		errorOut += "Bad Event Constructor (GetBindedFunctionCount)\n";
		result = false;
	}

	// ----------------- Bind static function test

	myEvent.Bind(&EventSystemTest::EventFunction);

	if (!Compare(myEvent.GetBindedFunctionCount(), 1))
	{
		errorOut += "Bad Event Bind (GetBindedFunctionCount)\n";
		result = false;
	}

	// Try to bind twice the same function, should not bind it twice
	myEvent.Bind(&EventSystemTest::EventFunction);

	if (!Compare(myEvent.GetBindedFunctionCount(), 1))
	{
		errorOut += "Bad Event Bind (GetBindedFunctionCount), binded twice\n";
		result = false;
	}

	myEvent.Trigger(eventValue); //1
	myEvent.Trigger(eventValue); //2
	myEvent.Trigger(eventValue); //3

	if (!Compare(eventValue, 3))
	{
		errorOut += "Bad Event Trigger\n";
		result = false;
	}

	myEvent.Unbind(&EventSystemTest::EventFunction);

	if (!Compare(myEvent.GetBindedFunctionCount(), 0))
	{
		errorOut += "Bad Event UnBind (GetBindedFunctionCount)\n";
		result = false;
	}

	// Try to unbind a function that is not binded, should not do anything
	myEvent.Unbind(&EventSystemTest::EventFunction);

	if (!Compare(myEvent.GetBindedFunctionCount(), 0))
	{
		errorOut += "Bad Event UnBind (GetBindedFunctionCount)\n";
		result = false;
	}

	// ----------------- Bind object function test

	myEvent.Bind(&EventSystemTest::EventObjectFunction, this);

	if (!Compare(myEvent.GetBindedFunctionCount(), 1))
	{
		errorOut += "Bad Event Bind Object Function (GetBindedFunctionCount)\n";
		result = false;
	}

	// Try to bind twice the same function, should not bind it twice
	myEvent.Bind(&EventSystemTest::EventObjectFunction, this);

	if (!Compare(myEvent.GetBindedFunctionCount(), 1))
	{
		errorOut += "Bad Event Bind Object Function (GetBindedFunctionCount), binded twice\n";
		result = false;
	}

	myEvent.Trigger(eventValue); // 6
	myEvent.Trigger(eventValue); // 12

	if (!Compare(eventValue, 12))
	{
		errorOut += "Bad Event Trigger with Object Function\n";
		result = false;
	}

	// ----------------- UnbindAll test

	myEvent.UnbindAll();

	if (!Compare(myEvent.GetBindedFunctionCount(), 0))
	{
		errorOut += "Bad Event UnbindAll (GetBindedFunctionCount)\n";
		result = false;
	}

	myEvent.Trigger(eventValue); // 12

	if (!Compare(eventValue, 12))
	{
		errorOut += "Bad Event Trigger after UnbindAll\n";
		result = false;
	}

	return result;
}