// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <string>
#include <mutex>

#include <editor/ui/menus/menu.h>
#include <engine/engine_args.h>

class DevKitControlMenu : public Menu
{
public:
	void Init() override;
	void Draw() override;

private:
	enum class DevKitError
	{
		NoError,
		WaitingForResponse,
		FailedToPowerOn,
		FailedToPowerOff,
		FailedToReset,
		FailedToGetList,
		FailedToLaunchGame,
	};

	struct DevKit
	{
		std::string name;
		std::string ip;
	};

	void PowerOnDevKit();
	void PowerOffDevKit();
	void ResetDevKit();
	void GetDevKitList();
	void LaunchGame(DevKitRunningMode devKitRunningMode);
	void StartLogListening();

	std::mutex m_devKitListMutex;
	std::vector<DevKit> m_devKits;
	DevKitError m_currentError = DevKitError::NoError;
	int m_selectedDevKit = -1;
};

