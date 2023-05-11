#pragma once

#include <functional>
#include <vector>

using EventCallback = std::function<void()>;

class EventSystem
{
public:
	void operator+=(const EventCallback& callback) 
	{
		callbacks.push_back(callback);
	}

	void triggerEvents() 
	{
		for (const auto& callback : callbacks)
		{
			callback();
		}
	}

private:
	std::vector<EventCallback> callbacks;
};

