#include "pch.h"
#include "Matrix44.h"
#include <cmath>

#define PI 3.14159265

Matrix44::Matrix44()
{
}

Matrix44::Matrix44(float x11, float x12, float x13, float x14,
	               float x21, float x22, float x23, float x24,
	               float x31, float x32, float x33, float x34,
	               float x41, float x42, float x43, float x44) {
	Vector4 row1(x11, x12, x13, x14);
	m_Matrix[0] = row1;
	Vector4 row2(x21, x22, x23, x24);
	m_Matrix[1] = row2;
	Vector4 row3(x31, x32, x33, x34);
	m_Matrix[2] = row3;
	Vector4 row4(x41, x42, x43, x44);
	m_Matrix[3] = row4;

}

Matrix44::Matrix44(Vector4 row1, Vector4 row2, Vector4 row3, Vector4 row4) {
	m_Matrix[0] = row1;
	m_Matrix[1] = row2;
	m_Matrix[2] = row3;
	m_Matrix[3] = row4;
}

Matrix44::Matrix44(Vector4* p_Matrix) {
	m_Matrix[0] = p_Matrix[0];
	m_Matrix[1] = p_Matrix[1];
	m_Matrix[2] = p_Matrix[2];
	m_Matrix[3] = p_Matrix[3];
}

Matrix44::~Matrix44()
{
}


Matrix44& Matrix44::operator+=(Matrix44 other) {
	m_Matrix[0] += other[0];
	m_Matrix[1] += other[1];
	m_Matrix[2] += other[2];
	m_Matrix[3] += other[3];
	return *this;
}

Matrix44& Matrix44::operator-=(Matrix44 other) {
	m_Matrix[0] -= other[0];
	m_Matrix[1] -= other[1];
	m_Matrix[2] -= other[2];
	m_Matrix[3] -= other[3];
	return *this;
}

Matrix44& Matrix44::operator*=(float n) {
	m_Matrix[0] *= n;
	m_Matrix[1] *= n;
	m_Matrix[2] *= n;
	m_Matrix[3] *= n;
	return *this;
}

Matrix44& Matrix44::operator/=(float n) {
	try {
		if (n == 0) {
			throw n;
		}
		m_Matrix[0] /= n;
		m_Matrix[1] /= n;
		m_Matrix[2] /= n;
		m_Matrix[3] /= n;
		return *this;
	}
	catch (int e_n) {
		std::cout << "Invalid divisor " << e_n << std::endl;
		return *this;
	}
}

Matrix44& Matrix44::operator=(Matrix44& other) {
	m_Matrix[0] = other[0];
	m_Matrix[1] = other[1];
	m_Matrix[2] = other[2];
	m_Matrix[3] = other[3];
	return *this;
}

Matrix44& Matrix44::operator*=(Matrix44 other) {
	Vector4 temp[4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i][j] = m_Matrix[i] * other.GetColumn(j);
		}
		
	}
	for (int i = 0; i < 4; i++) {
		m_Matrix[i] = temp[i];
	}
	return *this;
}

void swap(float& first, float& second) {
	float temp = first;
	first = second;
	second = temp;
}

Matrix44 Matrix44::transpose() {
	Matrix44 temp = (*this);
	for (int i = 0; i < 4; i++) {
		for (int j = i; j < 4; j++) {
			swap(temp[i][j], temp[j][i]);
		}
	}
	return temp;
}

Matrix44 Matrix44::inverseR() {
	Matrix44 rot = (*this);
	Vector4 translation(rot[3][0], rot[3][1], rot[3][2], 0);
	for (int i = 0; i < 4; i++) {
		rot[3][i] = 0;
		rot[i][3] = 0;
	}
	Matrix44 trans_rot = rot.transpose();
	Vector4 new_trans = -translation * trans_rot;
	for (int i = 0; i < 3; i++) {
		trans_rot[3][i] = new_trans[i];
	}
	trans_rot[3][3] = 1;
	return trans_rot;
}

Matrix44 Matrix44::CreateTranslationR(Vector4 coordinate) {
	return Matrix44(1, 0, 0, 0,
		            0, 1, 0, 0,
		            0, 0, 1, 0,
		            coordinate.X(), coordinate.Y(), coordinate.Z(), 1);
}

Matrix44 Matrix44::CreateZRotationR(float angle) {
	float param = (float)PI / 180.0f;
	return Matrix44(cos(angle * param), sin(angle * param), 0, 0,
		            - sin(angle * param), cos(angle * param), 0, 0,
		            0, 0, 1, 0,
		            0, 0, 0, 1);
}

Matrix44 Matrix44::CreateScale(Vector4 scale) {
	return Matrix44(scale.X(), 0, 0, 0,
		            0, scale.Y(), 0, 0,
		            0, 0, scale.Z(), 0,
		            0, 0, 0, 1);
}