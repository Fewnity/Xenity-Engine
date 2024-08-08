#pragma once

#include <string>
#include <cstdint>

class ScopeBenchmark
{
public:
	ScopeBenchmark(const std::string& classAndFunction);
	~ScopeBenchmark();
private:
	std::string name;
	uint64_t start;
	uint32_t scopeLevel;
};

