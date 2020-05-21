#pragma once

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