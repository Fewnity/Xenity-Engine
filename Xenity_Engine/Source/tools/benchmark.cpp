#include "benchmark.h"

using namespace std::chrono;

void Benchmark::Start()
{
	start_point = high_resolution_clock::now();
}

void Benchmark::Stop()
{
	end_point = high_resolution_clock::now();

	auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
	auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();

	time = end - start;
}

int Benchmark::GetMicroSeconds()
{
	return time;
}

float Benchmark::GetSeconds() {

	return time / 1000000.0f;
}

int Benchmark::GetMilliseconds()
{
	return time / 1000.0f;
}

void Benchmark::Reset()
{
	time = 0;
}
