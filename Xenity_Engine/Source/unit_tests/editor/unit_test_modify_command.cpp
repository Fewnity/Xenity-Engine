// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

#include "../unit_test_manager.h"
#include <editor/command/commands/modify.h>

TestResult ModifyReflectiveCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	int variable = 10;
	int oldValue = variable;
	int newValue = 17;

	ReflectiveData reflectiveData;
	Reflective::AddVariable(reflectiveData, variable, "Variable");

	ReflectiveDataToDraw dataToDraw = ReflectiveDataToDraw();
	dataToDraw.reflectiveDataStack.push_back(reflectiveData);
	dataToDraw.ownerType = ReflectiveDataToDraw::OwnerTypeEnum::None;
	dataToDraw.currentEntry.variableName = "Variable";

	ReflectiveChangeValueCommand command = ReflectiveChangeValueCommand<int>(dataToDraw, &variable, oldValue, newValue);

	command.Execute();

	EXPECT_EQUALS(variable, newValue, "Variable has not been modified to the new value");

	command.Undo();

	EXPECT_EQUALS(variable, oldValue, "Variable has not been modified to the old value");

	command.Redo();

	EXPECT_EQUALS(variable, newValue, "Variable has not been modified to the new value");

	command.Undo();

	EXPECT_EQUALS(variable, oldValue, "Variable has not been modified to the old value");

	command.Redo();

	EXPECT_EQUALS(variable, newValue, "Variable has not been modified to the new value");

	EXPECT_EQUALS(newValue, 17, "New value has changed");
	EXPECT_EQUALS(oldValue, 10, "Old value has changed");

	END_TEST();
}

TestResult ModifyInspectorChangeValueCommandTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	////////////////////// No target //////////////////////

	int t = 3;
	std::weak_ptr<void> weakPtr;
	InspectorChangeValueCommand command = InspectorChangeValueCommand(weakPtr, &t, 1, 3);
	command.Execute();

	EXPECT_EQUALS(t, 1, "Value has not been modified to the new value");

	command.Undo();

	EXPECT_EQUALS(t, 3, "Value has not been modified to the older value");

	command.Redo();

	EXPECT_EQUALS(t, 1, "Value has not been modified to the new value");

	////////////////////// GameObject target //////////////////////

	////////////////////// Transform target //////////////////////

	////////////////////// FileReference target //////////////////////

	END_TEST();
}

#endif