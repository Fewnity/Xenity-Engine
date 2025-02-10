#pragma once

template<typename... Args>
class Event;

class UpdateChecker
{
public:
	/**
	* @brief Check for an update
	* @return True if an update is available
	*/
	static void CheckForUpdate(Event<bool>* onUpdateCheckedEvent);
};

