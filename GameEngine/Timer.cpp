#include "pch.h"
#include "Timer.h"
#include "Engine.h"
#include <Windows.h>

Timer* Timer::timer = nullptr;

Timer::Timer()
{
	registry = vector<GameObject*>();
	QueryPerformanceCounter(&pre_tick);
	LARGE_INTEGER int_frequency;
	QueryPerformanceFrequency(&int_frequency);
	frequency = static_cast<float>(int_frequency.QuadPart);
}

Timer* Timer::getInstance() {
	if (!timer) {
		timer = new Timer();
	}
	return timer;
}

Timer::~Timer()
{
	while (registry.size() != 0) {
		GameObject* object = registry.back();
		registry.pop_back();
		if(object) object = nullptr;
	}
}

void Timer::tick(float delta_time) {
	for (SmartPointer<GameObject> object : *(Engine::World_GameObject))
	{
		object->BeginUpdate(delta_time);
	}
	for (SmartPointer<GameObject> object : *(Engine::World_GameObject))
	{
		object->Update(delta_time);
	}
	
	// IMPORTANT: Tell GLib that we want to start rendering
	GLib::BeginRendering();
	// Tell GLib that we want to render some sprites
	GLib::Sprites::BeginRendering();
	for (SmartPointer<GameObject> object : *(Engine::World_GameObject))
	{
		object->EndUpdate(delta_time);
	}
	// Tell GLib we're done rendering sprites
	GLib::Sprites::EndRendering();
	// IMPORTANT: Tell GLib we're done rendering
	GLib::EndRendering();
	
}

void Timer::tick() {
	float delta_time = CalcLastFrameTime_ms();
	// float delta_time = 1;
	Engine::CollisionSystem::CheckCollision(delta_time);
}

void Timer::regist(GameObject* new_object) {
	registry.push_back(new_object);
}

void Timer::remove_from_registry(GameObject* to_be_removed) {

}

float Timer::CalcLastFrameTime_ms() {
	LARGE_INTEGER cur_tick;
	QueryPerformanceCounter(&cur_tick);

	LONGLONG delta_ticks = cur_tick.QuadPart - pre_tick.QuadPart;
	float delta_time = delta_ticks / frequency;
	pre_tick = cur_tick;
	return delta_time;
}