#pragma once
#include <iostream>
#include "HeapManager.h"
#include "HeapManagerProxy.h"
#include "FloatComparisonHelper.h"
#include "Point2D.h"

class Vector4
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

	Vector4();
	Vector4(const Vector4&);
	Vector4(Vector4&&) noexcept;
	Vector4(Point2D);
	Vector4(Point2D, float a);
	Vector4(float x, float y, float z, float a);
	~Vector4();

	void set_position(float, float, float = 1, float = 0);
	void set_position(const Vector4&);

	Vector4* operator+=(Vector4);
	Vector4* operator-=(Vector4);
	Vector4* operator*=(float);
	Vector4* operator/=(float);
	Vector4& operator= (const Vector4& otherPoint);
	float& operator[](int index);
	friend std::ostream& operator<<(std::ostream& output, const Vector4& point);

	Vector4 Normalize();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   

	const float X() const {
		return x;
	}
	const float Y() const {
		return y;
	}
	const float Z() const {
		return z;
	}
	const float A() const {
		return a;
	}
	void X(float x) {
		this->x = x;
	}
	void Y(float y) {
		this->y = y;
	}
	void Z(float z) {
		this->z = z;
	}
	void A(float a) {
		this->a = a;
	}

protected:
	float x;
	float y;
	float z;
	float a;

private:


};

// inside class definition
inline void Vector4::set_position(float add_x, float add_y, float add_z, float add_a) {
	x = add_x;
	y = add_y;
	z = add_z;
	a = add_a;
}

inline void Vector4::set_position(const Vector4& otherPoint) {
	x = otherPoint.X();
	y = otherPoint.Y();
	z = otherPoint.Z();
	a = otherPoint.A();
}

inline Vector4& Vector4::operator= (const Vector4& otherPoint) {
	set_position(otherPoint);
	return *this;
}

inline Vector4* Vector4::operator+=(const Vector4 otherPoint) {
	x += otherPoint.X();
	y += otherPoint.Y();
	z += otherPoint.Z();
	a += otherPoint.A();
	return this;
}

inline Vector4* Vector4::operator-=(const Vector4 otherPoint) {
	x -= otherPoint.X();
	y -= otherPoint.Y();
	z -= otherPoint.Z();
	a -= otherPoint.A();
	return this;
}

inline Vector4* Vector4::operator *= (float n) {
	x *= n;
	y *= n;
	z *= n;
	a *= n;
	return this;
}


//outside class definition
inline Vector4 operator+(const Vector4& onePoint, const Vector4& otherPoint) {
	return Vector4(onePoint.X() + otherPoint.X(), onePoint.Y() + otherPoint.Y(), onePoint.Z() + otherPoint.Z(), onePoint.A() + otherPoint.A());
}

inline Vector4 operator-(const Vector4& onePoint, const Vector4& otherPoint) {
	return Vector4(onePoint.X() - otherPoint.X(), onePoint.Y() - otherPoint.Y(), onePoint.Z() - otherPoint.Z(), onePoint.A() - otherPoint.A());
}

inline Vector4 operator*(const Vector4& onePoint, float n) {
	return Vector4(onePoint.X() * n, onePoint.Y() * n, onePoint.Z() * n, onePoint.A() * n);
}

inline Vector4 operator*(float n, const Vector4& onePoint) {
	return Vector4(onePoint.X() * n, onePoint.Y() * n, onePoint.Z() * n, onePoint.A() * n);
}

inline float operator*(const Vector4 onePoint, const Vector4 otherPoint) {
	float result = onePoint.X() * otherPoint.X() +
		onePoint.Y() * otherPoint.Y() +
		onePoint.Z() * otherPoint.Z() +
		onePoint.A() * otherPoint.A();
	return result;
}

inline Vector4 operator/(const Vector4& onePoint, float n) {
	try {
		if (n == 0) {
			throw n;
		}
		return Vector4(onePoint.X() / n, onePoint.Y() / n, onePoint.Z() / n, onePoint.A() / n);
	}
	catch (int e_n) {
		std::cout << "Invalid divisor " << e_n << std::endl;
		Vector4 empty;
		return empty;
	}

}

inline bool operator==(const Vector4& onePoint, const Vector4& otherPoint) {
	return (FloatEqual(onePoint.X(), otherPoint.X()) && FloatEqual(onePoint.Y(), otherPoint.Y()) && FloatEqual(onePoint.Z(), otherPoint.Z()) && FloatEqual(onePoint.A(), otherPoint.A()));
}

inline bool operator!=(const Vector4& onePoint, const Vector4& otherPoint) {
	return (!FloatEqual(onePoint.X(), otherPoint.X()) || !FloatEqual(onePoint.Y(), otherPoint.Y()) || !FloatEqual(onePoint.Z(), otherPoint.Z()) || !FloatEqual(onePoint.A(), otherPoint.A()));
}

inline Vector4 operator-(const Vector4& onePoint) {
	return Vector4(-onePoint.X(), -onePoint.Y(), -onePoint.Z(), -onePoint.A());
}

inline Vector4 operator+(const Vector4& onePoint) {
	return Vector4(onePoint.X(), onePoint.Y(), onePoint.Z(), onePoint.A());
}