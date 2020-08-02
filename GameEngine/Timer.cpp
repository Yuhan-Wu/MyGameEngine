#include "pch.h"
#include "Timer.h"
#include "Engine.h"
#include <Windows.h>

Timer* Timer::timer = nullptr;

Timer::Timer()
{
	QueryPerformanceCounter(&m_PreTick);
	LARGE_INTEGER int_frequency;
	QueryPerformanceFrequency(&int_frequency);
	m_Frequency = static_cast<float>(int_frequency.QuadPart);
}

Timer* Timer::getInstance() {
	if (!timer) {
		timer = new Timer();
	}
	return timer;
}

Timer::~Timer()
{
}

float Timer::CalcLastFrameTime_ms() {
	LARGE_INTEGER cur_tick;
	QueryPerformanceCounter(&cur_tick);

	LONGLONG delta_ticks = cur_tick.QuadPart - m_PreTick.QuadPart;
	float delta_time = delta_ticks / m_Frequency;
	m_PreTick = cur_tick;
	return delta_time;
}