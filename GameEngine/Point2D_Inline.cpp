#include "pch.h"
#include "Point2D.h"

#define POINT2D_INLINE

#ifndef POINT2D_INLINE

//inside class definition
inline void Point2D::set_position(double add_x, double add_y) {
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

inline Point2D* Point2D::operator*=(double n) {
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

inline Point2D operator*(const Point2D& onePoint, double n) {
	return Point2D(onePoint.X() * n, onePoint.Y() * n);
}

inline Point2D operator*(double n, const Point2D& onePoint) {
	return Point2D(onePoint.X() * n, onePoint.Y() * n);
}

inline Point2D operator/(const Point2D& onePoint, double n) {
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
	return ((onePoint.X() == otherPoint.X()) && (onePoint.Y() == otherPoint.Y()));
}

inline bool operator!=(const Point2D& onePoint, const Point2D& otherPoint) {
	return ((onePoint.X() == otherPoint.X()) && (onePoint.Y() == otherPoint.Y()));
}

inline Point2D operator-(const Point2D& onePoint) {
	return Point2D(-onePoint.X(), -onePoint.Y());
}

inline Point2D operator+(const Point2D& onePoint) {
	return Point2D(onePoint.X(), onePoint.Y());
}

#endif // 