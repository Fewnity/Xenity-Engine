#pragma once

#include <chrono>

class Benchmark
{
public:
	void Start();
	void Stop();
	int GetMicroSeconds();
	int GetMilliseconds();
	float GetSeconds();
	void Reset();

private:
	std::chrono::time_point< std::chrono::high_resolution_clock> start_point, end_point;
	int time;
};

