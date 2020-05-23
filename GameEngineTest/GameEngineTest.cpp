// IrisGameEngineTest.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Point2D.h"
#include "GameObject.h"
#include "Engine.h"
#include "FloatComparisonHelper.h"
#include "SmartPointer.h"
#include "WeakPointer.h"
#include "Vector4.h"
#include "Matrix44.h"
#include "PhysicsInfo.h"
#include "BoxCollision.h"
#include "GLib.h"

#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <math.h>

void Point2D_UnitTest()
{
	Point2D P1(0.0f, 0.0f);
	Point2D P2(0.0f, 0.0f);

	Point2D P3 = P1 + P2;
	P3 = P1 - P2;

	Point2D P4 = P1 * 0.5f;

	Point2D P5 = P1 / 2.0f;

	Point2D P6 = -P4;

	P6 *= 2.0f;
	P5 /= 4.0f;

	P2 += P1;
	P3 -= P1;

	Point2D P7 = (((P1 + P2) * 2.0f) - -P3) / 2.0f;

	bool bArentEqual = P6 != P4;

	cout << bArentEqual << endl;
}

void ConstCorrectTest() {
	const Point2D P1(0.0f, 0.0f);
	Point2D P2(0.0f, 0.0f);

	Point2D P3 = P1 + P2;
	P3 = P1 - P2;

	const Point2D P4 = P1 * 0.5f;

	Point2D P5 = P1 / 2.0f;

	Point2D P6 = -P4;

	P6 *= 2.0f;
	P5 /= 4.0f;

	P2 += P1;
	P3 -= P1;

	Point2D P7 = (((P1 + P2) * 2.0f) - -P3) / 2.0f;

	bool bArentEqual = P6 != P4;

	cout << bArentEqual << endl;

	Point2D P8;
	P8.set_position(P1);
}

void FloatComparisonTest() {
	float a = 0.1f;
	float b = 0;
	for (auto i = 0; i < 10; i++) {
		b += a;
	}
	assert(FloatEqual(a * 10, b));
	assert(NanCompare(NAN));
	assert(ZeroCompare(+0.0 - 0.0));
}

void SmartPointerTest() {
	SmartPointer<GameObject> base = GameObject::Create("Default", Point2D(0, 0), 0);
	SmartPointer<GameObject> temp1;
	assert(!temp1);
	assert(temp1 == nullptr);
	SmartPointer<GameObject> temp2(base);
	temp1 = temp2;
	assert(temp1 == temp2);
	temp2 = nullptr;
	assert(nullptr != temp1);
	assert(temp1->m_Position2D.X() == 0);
	SmartPointer<GameObject> temp3 = SmartPointer<GameObject>(base);
	assert(temp3 == temp1);
	temp1.PrintCounter();
	temp1 = nullptr;
	temp3 = nullptr;

	base.PrintCounter();

	base = nullptr;
	base.PrintCounter();

}

void WeakPointerTest() {
	Point2D* test1 = new Point2D(0.0f, 0.0f);
	Point2D* test2 = new Point2D(1.0f, 1.0f);
	SmartPointer<Point2D> temp1(test1);
	WeakPointer<Point2D> temp2;

	temp2 = temp1;
	SmartPointer<Point2D> temp3(temp2);
	assert(temp1 == temp3);
	temp1.PrintCounter();

	temp2 = nullptr;
	assert(temp1);
	temp1.PrintCounter();

	temp2 = temp1;
	temp1 = nullptr;
	temp3 = nullptr;
	SmartPointer<Point2D> temp4 = temp2.Get();
	assert(!temp4);

	delete test1;
	delete test2;
}

void Vector4Test() {
	Vector4 vec1(1, 2, 3, 4);
	Vector4 vec2 = vec1;
	assert(vec1 == vec2);

	float result = vec1 * vec2;
	assert(result == 30);

	Vector4 vec3 = vec1 + vec2;
	assert(vec3 == 2 * vec1);

	Vector4 vec4 = vec1 - vec2;
	assert(vec4 == Vector4(0, 0, 0, 0));
	assert(vec4 != vec1);

	vec1 *= 2;
	assert(vec1 == vec3);

	vec2 += vec2;
	assert(vec2 == vec3);
}

void Matrix44Test() {
	Matrix44 diagnol(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	Matrix44 m1_r(0, 1, 0, 0,
		-1, 0, 0, 0,
		0, 0, 1, 0,
		1, 2, 3, 1);
	assert(diagnol * m1_r == m1_r);

	Matrix44 m2_r = m1_r;
	assert(m2_r == m1_r);

	Matrix44 m3_r(0, -1, 0, 1,
		1, 0, 0, 2,
		0, 0, 1, 3,
		0, 0, 0, 1);
	assert(m3_r == m1_r.transpose());

	Matrix44 m4_l(0, 1, 0, -2,
		-1, 0, 0, 1,
		0, 0, 1, -3,
		0, 0, 0, 1);
	assert(m4_l * m3_r == diagnol);
	assert(m3_r.transpose().inverseR() == m4_l.transpose());
	
	Vector4 vec1(1, 2, 3, 1);
	Matrix44 m5_r(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 2, 3, 1);
	assert(m5_r == Matrix44::CreateTranslationR(vec1));

	Matrix44 m6_r(0, 1, 0, 0,
		-1, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	assert(m6_r == Matrix44::CreateZRotationR(90));

	assert(m6_r * m5_r == m1_r);

	Vector4 vec2(1, 0, 0, 1);
	Vector4 vec3(1, 3, 3, 1);
	assert(vec2 * m1_r == vec3);
	
	Matrix44 m7_r = diagnol * 2;
	m7_r -= Matrix44(0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 1);
	assert(m7_r == Matrix44::CreateScale(Vector4(2, 2, 2, 1)));
}

void CollisionTest() {
	// initialize
	// Collision
	Engine::start();
	/*
	SmartPointer<GameObject> A = GameObject::Create("A", 2, Point2D(0, 0), 0);
	PhysicsInfo* PhysicsA = new PhysicsInfo(A, 1, Point2D(0, 1.5), true);
	BoxCollision* CollisionA = new BoxCollision(A, Point2D(0, 0), Point2D(1, 1),"Pawn");
	A->Attach(PhysicsA);
	A->Attach(CollisionA);
	Engine::World_GameObject->push_back(A);

	SmartPointer<GameObject> B = GameObject::Create("B", 2, Point2D(0, 2.5), 0);
	PhysicsInfo* PhysicsB = new PhysicsInfo(B, 1, Point2D(0, 0), false);
	BoxCollision* CollisionB = new BoxCollision(B, Point2D(0, 0), Point2D(1, 1), "Static");
	B->Attach(PhysicsB);
	B->Attach(CollisionB);
	Engine::World_GameObject->push_back(B);

	bool collided = false;
	int counter = 1;
	for (int i = 0; i < counter; i++) {
		Engine::tick();
	}
	*/
	Engine::clean();
	
}

using namespace std;
int main()
{
	/*
	Point2D_UnitTest();
	ConstCorrectTest();
	FloatComparisonTest();
	Vector4Test();
	Matrix44Test();
	CollisionTest();*/

	SmartPointerTest();
	// WeakPointerTest();
}