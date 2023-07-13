#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <chrono>

class API Benchmark
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

