#pragma once

#include "GameObject.h"
#include "GLib.h"

class Timer
{
public:
	static Timer* getInstance();
	~Timer();

	float CalcLastFrameTime_ms();

private:
	static Timer* timer;
	Timer();

	LARGE_INTEGER m_PreTick;
	float m_Frequency;
};