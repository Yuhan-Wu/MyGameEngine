#pragma once

//inside class definition
inline void Point2D::set_position(float add_x, float add_y) {
	x = add_x;
	y = add_y;
}

inline void Point2D::set_position(const Point2D& otherPoint) {
	x = otherPoint.X();
	y = otherPoint.Y();
}

inline Point2D& Point2D::operator= (Point2D otherPoint) {
	set_position(otherPoint);
	return *this;
}

inline Point2D& Point2D::operator+=(Point2D otherPoint) {
	x += otherPoint.X();
	y += otherPoint.Y();
	return *this;
}

inline Point2D& Point2D::operator-=(Point2D otherPoint) {
	x -= otherPoint.X();
	y -= otherPoint.Y();
	return *this;
}

inline Point2D& Point2D::operator *= (float n) {
	x *= n;
	y *= n;
	return *this;
}


//outside class definition
inline Point2D operator+(Point2D onePoint, const Point2D otherPoint) {
	return Point2D(onePoint.X() + otherPoint.X(), onePoint.Y() + otherPoint.Y());
}

inline Point2D operator-(Point2D onePoint, const Point2D otherPoint) {
	return Point2D(onePoint.X() - otherPoint.X(), onePoint.Y() - otherPoint.Y());
}

inline Point2D operator*(Point2D onePoint, float n) {
	return Point2D(onePoint.X() * n, onePoint.Y() * n);
}

inline Point2D operator*(float n, Point2D onePoint) {
	return Point2D(onePoint.X() * n, onePoint.Y() * n);
}

inline Point2D operator/(Point2D onePoint, float n) {
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

inline bool operator==(Point2D const& onePoint, Point2D const& otherPoint) {
	return (FloatEqual(onePoint.X(), otherPoint.X()) && FloatEqual(onePoint.Y(), otherPoint.Y()));
}

inline bool operator!=(Point2D const& onePoint, Point2D const& otherPoint) {
	return (!FloatEqual(onePoint.X(), otherPoint.X()) || !FloatEqual(onePoint.Y(), otherPoint.Y()));
}

inline Point2D operator-(Point2D onePoint) {
	return Point2D(-onePoint.X(), -onePoint.Y());
}

inline Point2D operator+(Point2D onePoint) {
	return Point2D(onePoint.X(), onePoint.Y());
}