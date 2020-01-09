#include "BladesTimer.h"

using namespace std::chrono;

BladesTimer::BladesTimer() noexcept
{
	last = steady_clock::now();
}

float BladesTimer::Mark() noexcept
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float BladesTimer::Peek() const noexcept
{
	return duration<float>(steady_clock::now() - last).count();
}