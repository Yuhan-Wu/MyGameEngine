#pragma once

#include "Vector4.h"
#include <vector>
#include <iostream>

class Matrix44{
public:
	Matrix44();
	Matrix44(float x11, float x12, float x13, float x14,
		float x21, float x22, float x23, float x24,
		float x31, float x32, float x33, float x34,
		float x41, float x42, float x43, float x44);
	Matrix44(Vector4 row1, Vector4 row2, Vector4 row3, Vector4 row4);
	Matrix44(Vector4* p_Matrix);
	~Matrix44();
	
	Matrix44& operator+=(Matrix44);
	Matrix44& operator-=(Matrix44);
	Matrix44& operator*=(float);
	Matrix44& operator*=(Matrix44);
	Matrix44& operator/=(float);
	Matrix44& operator= (Matrix44& otherPoint);
	Vector4& operator[](int index) {
		return m_Matrix[index];
	}

	Vector4 GetColumn(int index) {
		return Vector4(m_Matrix[0][index], m_Matrix[1][index], m_Matrix[2][index], m_Matrix[3][index]);
	}
	Matrix44 transpose();
	Matrix44 inverseR();
	static Matrix44 CreateTranslationR(Vector4 coordinate);
	static Matrix44 CreateZRotationR(float angle); // around z axis
	static Matrix44 CreateScale(Vector4 scale);

	static Matrix44 I() {
		return Matrix44(1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);
	}
private:
	Vector4 m_Matrix[4];
};

#include "Matrix44-inln.h"