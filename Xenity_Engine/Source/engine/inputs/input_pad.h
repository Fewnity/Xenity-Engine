#pragma once

struct InputPad
{
	int buttons = 0;
	std::map<int, bool> pressedButtons;
	float lx = 0;
	float ly = 0;
	float rx = 0;
	float ry = 0;
};