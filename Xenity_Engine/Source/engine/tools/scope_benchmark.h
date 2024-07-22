#pragma once

#include <string>
#include <cstdint>
#include <chrono>

class ScopeBenchmark
{
public:
	ScopeBenchmark(const std::string& classAndFunction);
	~ScopeBenchmark();
private:
	std::string name;
	//std::chrono::time_point<std::chrono::high_resolution_clock> start;
	uint64_t start;
	uint32_t scopeLevel;
};

