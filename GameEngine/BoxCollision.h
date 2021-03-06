#pragma once

#include "GameObject.h"
#include "Matrix44.h"
#include "CollisionChannel.h"

class BoxCollision : public IGameObjectComponent
{
public:
	struct CollisionData {
		Point2D Center;
		Point2D Extent;
		CollisionData() {
			Center = Point2D(0, 0);
			Extent = Point2D(0, 0);
		}
		CollisionData(Point2D p_Center, Point2D p_Extent) {
			Center = p_Center;
			Extent = p_Extent;
		}
	};
	BoxCollision();
	BoxCollision(SmartPointer<GameObject>, Point2D, Point2D, std::string);
	~BoxCollision();

	Matrix44 GetMatrixToWorld();
	Point2D GetCenter();
	Point2D GetExtents();
	Channel GetChannel();
	bool GetCollided();
	void ReleaseExtra() override;

	void SetCollided(bool);

	ComponentType Name() override {
		return ComponentType::BoxCollision;
	}

private:
	SmartPointer<GameObject> m_GameObject;
	CollisionData m_Data;
	Channel m_Channel;
	bool m_Collided;
};