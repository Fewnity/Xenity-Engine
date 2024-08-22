#pragma once

#include <string>
#include <cstdint>

class ScopeBenchmark
{
public:
	ScopeBenchmark(const std::string& classAndFunction);
	~ScopeBenchmark();

private:
	std::string m_name;
	uint64_t m_start;
	uint32_t m_scopeLevel;
};

