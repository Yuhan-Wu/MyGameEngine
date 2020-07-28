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

	Vector4& operator+=(Vector4);
	Vector4& operator-=(Vector4);
	Vector4& operator*=(float);
	Vector4& operator/=(float);
	Vector4& operator= (Vector4 otherPoint);
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

#include "Vector4-inln.h"