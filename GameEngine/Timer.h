#pragma once

#include "GameObject.h"
#include "GLib.h"

class Timer
{
public:
	static Timer* getInstance();
	~Timer();

	void tick(float);
	void tick();
	void regist(GameObject*);
	void remove_from_registry(GameObject*);
	float CalcLastFrameTime_ms();

private:
	std::vector<GameObject*> registry;
	static Timer* timer;
	Timer();

	LARGE_INTEGER pre_tick;
	float frequency;
};