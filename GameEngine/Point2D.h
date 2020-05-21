#pragma once
#include <iostream>
#include "HeapManager.h"
#include "HeapManagerProxy.h"
#include "FloatComparisonHelper.h"
using namespace std;

class Point2D
{
public:
	static void* operator new(size_t size) {
		using namespace HeapManagerProxy;
		return alloc(HeapManager::GetInstance(), size);
	}
	static void* operator new(size_t size, unsigned int align) {
		using namespace HeapManagerProxy;
		return alloc(HeapManager::GetInstance(), size, align);
	}
	static void* operator new[](size_t size) {
		using namespace HeapManagerProxy;
		return alloc(HeapManager::GetInstance(), size);
	}

		static void operator delete(void* pointer) {
		using namespace HeapManagerProxy;
		free(HeapManager::GetInstance(), pointer);
	}
	static void operator delete[](void* pointer) {
		using namespace HeapManagerProxy;
		free(HeapManager::GetInstance(), pointer);
	}

	Point2D();
	Point2D(float x, float y);
	~Point2D();

	void set_position(float, float);
	void set_position(const Point2D&);

	Point2D* operator+=(Point2D);
	Point2D* operator-=(Point2D);
	Point2D* operator*=(float);
	Point2D* operator/=(float);
	Point2D& operator= (const Point2D& otherPoint);
	float& operator[](int index);
	friend ostream& operator<<(ostream& output, const Point2D& point);

	const float X() const {
		return x;
	}
	const float Y() const {
		return y;
	}
	void X(float x) {
		this->x = x;
	}
	void Y(float y) {
		this->y = y;
	}

protected:
	float x;
	float y;

private:

};

#include "Point2D-inln.h"