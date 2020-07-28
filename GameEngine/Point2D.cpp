#include "pch.h"
#include "Point2D.h"
#include <iostream>

Point2D::Point2D()
{
	this->x = 0;
	this->y = 0;
}

Point2D::Point2D(float x, float y) {
	Point2D::x = x;
	Point2D::y = y;
}

Point2D::~Point2D()
{
}

Point2D& Point2D::operator/= (float n) {
	try {
		if (n == 0) {
			throw n;
		}
		x /= n;
		y /= n;
		return *this;
	}
	catch (int e_n) {
		cout << "Invalid divisor " << e_n << endl;
		return *this;
	}

}

float& Point2D::operator[](int index) {
	try {
		if (index == 0) {
			return x;
		}
		else if (index == 1) {
			return y;
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

ostream& operator<<(ostream& output, const Point2D& point) {
	output << "(" << point.X() << "," << point.Y() << ")";
	return output;
}
