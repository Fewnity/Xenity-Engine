#pragma once

#include <string>
#include <cstdint>

class ScopeBenchmark
{
public:
	ScopeBenchmark(const size_t hash);
	ScopeBenchmark(const ScopeBenchmark& other) = delete;
	ScopeBenchmark& operator=(const ScopeBenchmark&) = delete;
	~ScopeBenchmark();

private:
	size_t m_hash;
	uint64_t m_start;
	uint32_t m_scopeLevel;
};

