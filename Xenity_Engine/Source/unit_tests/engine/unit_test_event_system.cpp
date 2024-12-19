// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/event_system/event_system.h>

void EventSystemTest::EventFunction(int& value) 
{
	value++;
}

void EventSystemTest::EventObjectFunction(int& value) 
{
	value *= 2;
}

TestResult EventSystemTest::Start(std::string& errorOut)
{
	BEGIN_TEST();
	int eventValue = 0;

	Event<int&> myEvent;

	// ----------------- Constructor test
	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 0, "Bad Event Constructor (GetBindedFunctionCount)");

	// ----------------- Bind static function test
	myEvent.Bind(&EventSystemTest::EventFunction);
	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 1, "Bad Event Bind (GetBindedFunctionCount)");

	// Try to bind twice the same function, should not bind it twice
	myEvent.Bind(&EventSystemTest::EventFunction);
	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 1, "Bad Event Bind (GetBindedFunctionCount), binded twice");

	myEvent.Trigger(eventValue); //1
	myEvent.Trigger(eventValue); //2
	myEvent.Trigger(eventValue); //3

	EXPECT_EQUALS(eventValue, 3, "Bad Event Trigger");

	myEvent.Unbind(&EventSystemTest::EventFunction);

	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 0, "Bad Event UnBind (GetBindedFunctionCount)");

	// Try to unbind a function that is not binded, should not do anything
	myEvent.Unbind(&EventSystemTest::EventFunction);

	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 0, "Bad Event UnBind (GetBindedFunctionCount), unbinded twice");

	// ----------------- Bind object function test

	myEvent.Bind(&EventSystemTest::EventObjectFunction, this);

	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 1, "Bad Event Bind Object Function (GetBindedFunctionCount)");

	// Try to bind twice the same function, should not bind it twice
	myEvent.Bind(&EventSystemTest::EventObjectFunction, this);

	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 1, "Bad Event Bind Object Function (GetBindedFunctionCount), binded twice");

	myEvent.Trigger(eventValue); // 6
	myEvent.Trigger(eventValue); // 12

	EXPECT_EQUALS(eventValue, 12, "Bad Event Trigger with Object Function");

	// ----------------- UnbindAll test

	myEvent.UnbindAll();

	EXPECT_EQUALS(myEvent.GetBindedFunctionCount(), 0, "Bad Event UnbindAll (GetBindedFunctionCount)");

	myEvent.Trigger(eventValue); // 12

	EXPECT_EQUALS(eventValue, 12, "Bad Event Trigger after UnbindAll");

	END_TEST();
}