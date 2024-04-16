#pragma once

struct InputPad
{
	std::map<int, bool> pressedButtons;
	float lx = 0;
	float ly = 0;
	float rx = 0;
	float ry = 0;
	int buttons = 0;
};