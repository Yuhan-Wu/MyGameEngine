#include "pch.h"
#include "Vector4.h"

Vector4::Vector4()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->a = 0;
}

Vector4::Vector4(const Vector4& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->a = other.a;
}

Vector4::Vector4(Vector4&& other) noexcept{
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	this->a = other.a;
}

Vector4::Vector4(Point2D location) {
	this->x = location.X();
	this->y = location.Y();
	this->z = 0;
	this->a = 0;
}

Vector4::Vector4(Point2D location, float a) {
	this->x = location.X();
	this->y = location.Y();
	this->z = 0;
	this->a = a;
}

Vector4::Vector4(float x, float y, float z, float a) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->a = a;
}

Vector4::~Vector4()
{
}

Vector4& Vector4::operator/= (float n) {
	try {
		if (n == 0) {
			throw n;
		}
		x /= n;
		y /= n;
		z /= n;
		a /= n;
		return *this;
	}
	catch (int e_n) {
		std::cout << "Invalid divisor " << e_n << std::endl;
		return *this;
	}

}

float& Vector4::operator[](int index) {
	try {
		if (index == 0) {
			return x;
		}
		else if (index == 1) {
			return y;
		}
		else if (index == 2) {
			return z;
		}
		else if (index == 3) {
			return a;
		}
		else {
			throw index;
		}
	}
	catch (int e_index) {
		std::cout << e_index << " is out of range." << std::endl;
	}
	return x;
}

std::ostream& operator<<(std::ostream& output, const Vector4& point) {
	output << "(" << point.X() << "," << point.Y() << "," << point.Z() << "," << point.A() << ")";
	return output;
}

Vector4 Vector4::Normalize() {
	float d = sqrt(x * x + y * y + z * z + a * a);
	return Vector4(x / d, y / d, z / d, a / d);
}