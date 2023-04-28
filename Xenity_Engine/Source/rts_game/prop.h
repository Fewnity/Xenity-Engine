#pragma once

class Prop {
public:
	int id = 0;
	int x = 0;
	int y = 0;

	void SetPosition(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};