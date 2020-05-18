#include "pch.h"
#include "PhysicsInfo.h"
#include "Timer.h"

PhysicsInfo::PhysicsInfo()
{
}

PhysicsInfo::PhysicsInfo(SmartPointer<GameObject> game_object, float mass, Point2D cur_vel, bool movablity) {
	this->game_object = game_object;
	this->data.mass = mass;
	this->data.movable = movablity;
	this->data.cur_vel = cur_vel;
	this->data.acceleration = Point2D(0, 0);
}

PhysicsInfo::~PhysicsInfo()
{
	game_object = nullptr;
}

Point2D PhysicsInfo::GetCurVel() {
	if (data.movable) {
		return data.cur_vel;
	}
	else {
		return Point2D(0, 0);
	}
	
}

void PhysicsInfo::SetCurVel(Point2D NewVel) {
	data.cur_vel = NewVel;
}

bool PhysicsInfo::GetMovability() {
	return data.movable;
}

float PhysicsInfo::GetMass() {
	return data.mass;
}

void PhysicsInfo::accelerate(Point2D force) {
	if (data.movable) {
		Point2D a = force / this->data.mass;
		data.acceleration = a;
	}
}

void PhysicsInfo::remove_force() {
	data.acceleration *= 0;
	data.cur_vel *= 0;
}

void PhysicsInfo::BeginUpdate(float delta_time) {
	if (data.movable) {
		*(game_object->position2D) += data.cur_vel * delta_time + 0.5 * data.acceleration * delta_time * delta_time;
		data.cur_vel += data.acceleration * delta_time;
	}
}

void PhysicsInfo::ReleaseExtra() {
	game_object = nullptr;
}
