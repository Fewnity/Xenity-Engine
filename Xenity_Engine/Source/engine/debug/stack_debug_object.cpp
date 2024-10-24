#include "stack_debug_object.h"

#include <engine/debug/debug.h>

StackDebugObject::StackDebugObject(const std::string& name, const uint32_t level) : m_name(name), m_level(level)
{
	if (m_level <= STACK_DEBUG_LEVEL)
	{
		Debug::Print("In " + m_name, true);
	}
}

StackDebugObject::~StackDebugObject()
{
	if (m_level <= STACK_DEBUG_LEVEL)
	{
		Debug::Print("Out " + m_name, true);
	}
}
