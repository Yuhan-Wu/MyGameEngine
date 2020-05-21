#include "pch.h"
#include "BoxCollision.h"

BoxCollision::BoxCollision()
{
	Point2D center(0, 0);
	Point2D extents(0, 0);
	CollisionData temp = CollisionData(center, extents);
	Data = temp;
	Matrix44 cons = Matrix44::I();
	collided = false;
	Channel = new std::string();
}

BoxCollision::BoxCollision(SmartPointer<GameObject> p_GameObject, Point2D p_Center, Point2D p_Extent, std::string channel) {
	CollisionData temp = CollisionData(p_Center, p_Extent);
	Data = temp;
	Channel = new std::string(channel);

	Matrix44 trans_AToWorld = Matrix44::CreateTranslationR(p_GameObject->m_Position2D);
	Matrix44 rot_AToWorld = Matrix44::CreateZRotationR(p_GameObject->m_RotZ);
	Matrix44 cons = trans_AToWorld * rot_AToWorld;

	Game_Object = p_GameObject;
	collided = false;
}

BoxCollision :: ~BoxCollision()
{
	delete Channel;
	Game_Object = nullptr;
}

Matrix44 BoxCollision::GetMatrixToWorld() {
	Matrix44 local_center = Matrix44::CreateTranslationR(Vector4(Game_Object->m_Position2D, 1.0f));
	Matrix44 rot_AToWorld = Matrix44::CreateZRotationR(Game_Object->m_RotZ);
	Matrix44 new_cons = rot_AToWorld * local_center;

	return new_cons;
}

Point2D BoxCollision::GetCenter() {
	return Data.Center;
}

Point2D BoxCollision::GetExtents() {
	return Data.Extent;
}

std::string BoxCollision::GetChannel() {
	return *(Channel);
}

bool BoxCollision::GetCollided() {
	return collided;
}

void BoxCollision::ReleaseExtra() {
	delete Channel;
	Game_Object = nullptr;
}

void BoxCollision::SetCollided(bool new_state) {
	collided = new_state;
}