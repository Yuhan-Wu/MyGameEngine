#pragma once

#include "GameData.h"
#include "CollisionChannel.h"
#include "MaskParser.h"

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
	unsigned int GetMask();
	void SetCollided(BoxCollision*);
	void CleanCollided();
	std::vector<BoxCollision*> GetCollided();
	void ReleaseExtra() override;
	
	ComponentType Name() override {
		return ComponentType::BoxCollision;
	}

private:
	SmartPointer<GameObject> m_GameObject;
	CollisionData m_Data;
	Channel m_Channel;
	unsigned int m_CollisionMask;
	std::vector<BoxCollision*> m_CollisionList;
};