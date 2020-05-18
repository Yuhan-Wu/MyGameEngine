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

//inside class definition
inline void Point2D::set_position(float add_x, float add_y) {
	x = add_x;
	y = add_y;
}

inline void Point2D::set_position(const Point2D& otherPoint) {
	x = otherPoint.X();
	y = otherPoint.Y();
}

inline Point2D& Point2D::operator= (const Point2D& otherPoint) {
	set_position(otherPoint);
	return *this;
}

inline Point2D* Point2D::operator+=(const Point2D otherPoint) {
	x += otherPoint.X();
	y += otherPoint.Y();
	return this;
}

inline Point2D* Point2D::operator-=(const Point2D otherPoint) {
	x -= otherPoint.X();
	y -= otherPoint.Y();
	return this;
}

inline Point2D* Point2D::operator *= (float n) {
	x *= n;
	y *= n;
	return this;
}


//outside class definition
inline Point2D operator+(const Point2D& onePoint, const Point2D& otherPoint) {
	return Point2D(onePoint.X() + otherPoint.X(), onePoint.Y() + otherPoint.Y());
}

inline Point2D operator-(const Point2D& onePoint, const Point2D& otherPoint) {
	return Point2D(onePoint.X() - otherPoint.X(), onePoint.Y() - otherPoint.Y());
}

inline Point2D operator*(const Point2D& onePoint, float n) {
	return Point2D(onePoint.X() * n, onePoint.Y() * n);
}

inline Point2D operator*(float n, const Point2D& onePoint) {
	return Point2D(onePoint.X() * n, onePoint.Y() * n);
}

inline Point2D operator/(const Point2D& onePoint, float n) {
	try {
		if (n == 0) {
			throw n;
		}
		return Point2D(onePoint.X() / n, onePoint.Y() / n);
	}
	catch (int e_n) {
		cout << "Invalid divisor " << e_n << endl;
		Point2D empty;
		return empty;
	}

}

inline bool operator==(const Point2D& onePoint, const Point2D& otherPoint) {
	return (FloatEqual(onePoint.X(), otherPoint.X()) && FloatEqual(onePoint.Y(), otherPoint.Y()));
}

inline bool operator!=(const Point2D& onePoint, const Point2D& otherPoint) {
	return (!FloatEqual(onePoint.X(), otherPoint.X()) || !FloatEqual(onePoint.Y(), otherPoint.Y()));
}

inline Point2D operator-(const Point2D& onePoint) {
	return Point2D(-onePoint.X(), -onePoint.Y());
}

inline Point2D operator+(const Point2D& onePoint) {
	return Point2D(onePoint.X(), onePoint.Y());
}