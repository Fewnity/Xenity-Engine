#pragma once

/**
 * [Internal]
 */

class CrashHandler
{
public:

	/**
	* Enable crash events
	*/
	static void Init();

	/**
	* Call a function in a try/catch
	* @param function Function to call
	*/
	static bool CallInTry(void (*function)());

private:

	/**
	* Called function on error (Not in Editor mode)
	*/
	static void Handler(int signum);
};

