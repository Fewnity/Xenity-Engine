#include "benchmark.h"

using namespace std::chrono;

/// <summary>
/// Start the benchmark timer
/// </summary>
void Benchmark::Start()
{
	start_point = high_resolution_clock::now();
}

/// <summary>
/// Stop the benchmark timer
/// </summary>
void Benchmark::Stop()
{
	end_point = high_resolution_clock::now();

	long long start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
	long long end = time_point_cast<microseconds>(end_point).time_since_epoch().count();

	time = end - start;
}

/// <summary>
/// Get how many microseconds passed during the benchmark
/// </summary>
/// <returns>Microseconds</returns>
long long Benchmark::GetMicroSeconds() const
{
	return time;
}

/// <summary>
/// Get how many milliseconds passed during the benchmark
/// </summary>
/// <returns>Milliseconds</returns>
long long Benchmark::GetMilliseconds() const
{
	return (long long)(time / 1000.0f);
}

/// <summary>
/// Get how many seconds passed during the benchmark
/// </summary>
/// <returns>Seconds</returns>
float Benchmark::GetSeconds() const
{
	return time / 1000000.0f;
}

/// <summary>
/// Reset the timer
/// </summary>
void Benchmark::Reset()
{
	time = 0;
}
