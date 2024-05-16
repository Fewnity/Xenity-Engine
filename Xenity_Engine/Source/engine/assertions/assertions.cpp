#include "assertions.h"

#include <engine/debug/debug.h>

void OnAssertionFailed(const std::string& message)
{
	Debug::PrintError("Assertion failed: " + message);
}
