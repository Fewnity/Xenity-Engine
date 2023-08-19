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

	/**
	* Start the benchmark
	*/
	void Start();

	/**
	* Calculate the elapsed time since the last Start call
	*/
	void Stop();

	/**
	* Get elapsed microseconds between Start and Stop calls
	*/
	long long GetMicroSeconds() const;

	/**
	* Get elapsed milliseconds between Start and Stop calls
	*/
	long long GetMilliseconds() const;

	/**
	* Get elapsed seconds between Start and Stop calls
	*/
	float GetSeconds() const;

	/**
	* Set times values to 0
	*/
	void Reset();

private:
	std::chrono::time_point< std::chrono::high_resolution_clock> start_point, end_point;
	long long time = 0;
};

