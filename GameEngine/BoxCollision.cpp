#include "pch.h"

#include "BoxCollision.h"

BoxCollision::BoxCollision()
{
	Point2D center(0, 0);
	Point2D extents(0, 0);
	CollisionData temp = CollisionData(center, extents);
	m_Data = temp;
	m_Channel = Channel::Static;
}

BoxCollision::BoxCollision(SmartPointer<GameObject> p_GameObject, Point2D p_Center, Point2D p_Extent, std::string channel) {
	CollisionData temp = CollisionData(p_Center, p_Extent);
	m_Data = temp;
	EnumParser<Channel> parser;
	m_Channel = parser.ParseSomeEnum(channel);

	m_GameObject = p_GameObject;
}

BoxCollision :: ~BoxCollision()
{
	m_GameObject = nullptr;
	m_CollisionList.clear();
}

Matrix44 BoxCollision::GetMatrixToWorld() {
	Matrix44 local_center = Matrix44::CreateTranslationR(Vector4(m_GameObject->m_Position2D, 1.0f));
	Matrix44 rot_AToWorld = Matrix44::CreateZRotationR(m_GameObject->m_RotZ);
	Matrix44 new_cons = rot_AToWorld * local_center;

	return new_cons;
}

Point2D BoxCollision::GetCenter() {
	return m_Data.Center;
}

Point2D BoxCollision::GetExtents() {
	return m_Data.Extent;
}

Channel BoxCollision::GetChannel() {
	return m_Channel;
}

std::vector<BoxCollision*> BoxCollision::GetCollided() {
	return m_CollisionList;
}

void BoxCollision::ReleaseExtra() {
	m_GameObject = nullptr;
	m_CollisionList.clear();
}

void BoxCollision::SetCollided(BoxCollision* p_OtherBox) {
	m_CollisionList.push_back(p_OtherBox);
}

void BoxCollision::CleanCollided() {
	m_CollisionList.clear();
}