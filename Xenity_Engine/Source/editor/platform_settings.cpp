// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "platform_settings.h"
#include <engine/graphics/texture.h>
#include <engine/file_system/file.h>
#include <stb_image.h>

int PlatformSettingsWindows::IsValid()
{

	return 0; // Valid
}

int PlatformSettingsPsVita::IsValid()
{
	if (backgroundImage)
	{
		if (backgroundImage->GetWidth() != 840 || backgroundImage->GetHeight() != 500)
		{
			return 1;
		}
	}
	if (iconImage)
	{
		if (iconImage->GetWidth() != 128 || iconImage->GetHeight() != 128)
		{
			return 2;
		}
	}
	if (startupImage)
	{
		if (startupImage->GetWidth() != 280 || startupImage->GetHeight() != 158)
		{
			return 3;
		}
	}

	const int gameIdLen = 9;
	if (gameId.size() != gameIdLen)
	{
		return 4;
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
				return 5;
			}
		}
	}

	return 0; // Valid
}

int PlatformSettingsPSP::IsValid()
{
	if (backgroundImage) 
	{
		if (backgroundImage->GetWidth() != 480 || backgroundImage->GetHeight() != 272)
		{
			return 1;
		}
	}
	if (iconImage)
	{
		if (iconImage->GetWidth() != 144 || iconImage->GetHeight() != 80)
		{
			return 2;
		}
	}
	if (previewImage)
	{
		if (previewImage->GetWidth() != 310 || previewImage->GetHeight() != 180)
		{
			return 3;
		}
	}

	return 0; // Valid
}
