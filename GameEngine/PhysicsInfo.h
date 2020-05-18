#pragma once

#include "GameObject.h"

class PhysicsInfo: public IGameObjectController
{
public:
	struct PhysicsData {
		float mass;
		bool movable;
		Point2D cur_vel;
		Point2D acceleration;
		PhysicsData() {
			mass = 0.0f;
			movable = true;
			cur_vel = Point2D(0, 0);
			acceleration = Point2D(0, 0);
		}
		PhysicsData(float p_mass, Point2D p_cur_vel, Point2D p_acceleration, bool movability) {
			mass = p_mass;
			movable = movability;
			cur_vel = p_cur_vel;
			acceleration = p_acceleration;
		}
	};
	PhysicsInfo();
	PhysicsInfo(SmartPointer<GameObject>, float, Point2D, bool);
	~PhysicsInfo();

	Point2D GetCurVel();
	void SetCurVel(Point2D);
	bool GetMovability();
	float GetMass();
	void accelerate(Point2D);
	void remove_force();
	void BeginUpdate(float) override;
	void ReleaseExtra() override;
	
	std::string Name() override{
		return "PhysicsInfo";
	}

private:
	SmartPointer<GameObject> game_object;
	PhysicsData data;
};
