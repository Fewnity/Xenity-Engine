#pragma once

class Plugin
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
};

