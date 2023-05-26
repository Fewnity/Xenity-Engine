#pragma once

#include <chrono>

class Benchmark
{
public:
	void Start();
	void Stop();
	long long GetMicroSeconds() const;
	long long GetMilliseconds() const;
	float GetSeconds() const;
	void Reset();

private:
	std::chrono::time_point< std::chrono::high_resolution_clock> start_point, end_point;
	long long time = 0;
};

