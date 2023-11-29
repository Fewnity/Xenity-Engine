#pragma once

class GameInterface
{
public:
	virtual ~GameInterface() = default;
	virtual void Start() = 0;
};