// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "platform_settings.h"

#include <engine/graphics/texture.h>

int PlatformSettingsWindows::IsValid()
{
	return static_cast<int>(PlatformSettingsErrorWindows::None); // Valid
}

int PlatformSettingsPS3::IsValid()
{
	return static_cast<int>(PlatformSettingsErrorPS3::None); // Valid
}

int PlatformSettingsPsVita::IsValid()
{
	if (backgroundImage)
	{
		if (backgroundImage->GetWidth() != 840 || backgroundImage->GetHeight() != 500)
		{
			return static_cast<int>(PlatformSettingsErrorPsVita::WrongBackgroundSize);
		}
	}
	if (iconImage)
	{
		if (iconImage->GetWidth() != 128 || iconImage->GetHeight() != 128)
		{
			return static_cast<int>(PlatformSettingsErrorPsVita::WrongIconSize);
		}
	}
	if (startupImage)
	{
		if (startupImage->GetWidth() != 280 || startupImage->GetHeight() != 158)
		{
			return static_cast<int>(PlatformSettingsErrorPsVita::WrongStartupImageSize);
		}
	}

	const int gameIdLen = 9;
	if (gameId.size() != gameIdLen)
	{
		return static_cast<int>(PlatformSettingsErrorPsVita::WrongGameIdSize);
	}
	else 
	{
		for (int i = 0; i < gameIdLen; i++)
		{
			if ((gameId[i] >= 'A' && gameId[i] <= 'Z') || (gameId[i] >= '0' && gameId[i] <= '9'))
			{
				// OK
			}
			else
			{
				return static_cast<int>(PlatformSettingsErrorPsVita::WrongGameId);
			}
		}
	}

	return static_cast<int>(PlatformSettingsErrorPsVita::None); // Valid
}

int PlatformSettingsPSP::IsValid()
{
	if (backgroundImage) 
	{
		if (backgroundImage->GetWidth() != 480 || backgroundImage->GetHeight() != 272)
		{
			return static_cast<int>(PlatformSettingsErrorPSP::WrongBackgroundSize);
		}
	}
	if (iconImage)
	{
		if (iconImage->GetWidth() != 144 || iconImage->GetHeight() != 80)
		{
			return static_cast<int>(PlatformSettingsErrorPSP::WrongIconSize);
		}
	}
	if (previewImage)
	{
		if (previewImage->GetWidth() != 310 || previewImage->GetHeight() != 180)
		{
			return static_cast<int>(PlatformSettingsErrorPSP::WrongPreviewSize);
		}
	}

	return static_cast<int>(PlatformSettingsErrorPSP::None); // Valid
}
