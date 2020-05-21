#pragma once

//outside class definition
inline Matrix44 operator+(Matrix44& one, Matrix44& other) {

	Vector4 row1 = one[0] + other[0];
	Vector4 row2 = one[1] + other[1];
	Vector4 row3 = one[2] + other[2];
	Vector4 row4 = one[3] + other[3];

	return Matrix44(row1, row2, row3, row4);
}

inline Matrix44 operator-(Matrix44& one, Matrix44& other) {

	Vector4 row1 = one[0] - other[0];
	Vector4 row2 = one[1] - other[1];
	Vector4 row3 = one[2] - other[2];
	Vector4 row4 = one[3] - other[3];


	return Matrix44(row1, row2, row3, row4);
}

inline Matrix44 operator*(Matrix44& onePoint, float n) {
	return Matrix44(onePoint[0] * n, onePoint[1] * n, onePoint[2] * n, onePoint[3] * n);
}

inline Matrix44 operator*(float n, Matrix44& onePoint) {
	return Matrix44(onePoint[0] * n, onePoint[1] * n, onePoint[2] * n, onePoint[3] * n);
}

inline Vector4 operator*(Matrix44& onePoint, Vector4 vector) {
	float x = onePoint[0] * vector;
	float y = onePoint[1] * vector;
	float z = onePoint[2] * vector;
	float a = onePoint[3] * vector;

	return Vector4(x, y, z, a);
}

inline Vector4 operator*(Vector4 vector, Matrix44& onePoint) {
	float x = onePoint.GetColumn(0) * vector;
	float y = onePoint.GetColumn(1) * vector;
	float z = onePoint.GetColumn(2) * vector;
	float a = onePoint.GetColumn(3) * vector;

	return Vector4(x, y, z, a);
}

inline Matrix44 operator*(Matrix44& one, Matrix44& other) {
	Matrix44 result;
	for (int i = 0; i < 4; i++) {
		Vector4 curRow;
		for (int j = 0; j < 4; j++) {
			curRow[j] = one[i] * other.GetColumn(j);
		}
		result[i] = curRow;
	}
	return result;
}

inline Matrix44 operator/(Matrix44& onePoint, float n) {
	try {
		if (n == 0) {
			throw n;
		}
		return Matrix44(onePoint[0] / n, onePoint[1] / n, onePoint[2] / n, onePoint[3] / n);
	}
	catch (int e_n) {
		std::cout << "Invalid divisor " << e_n << std::endl;
		Matrix44 empty;
		return empty;
	}

}

inline bool operator==(Matrix44 onePoint, Matrix44 otherPoint) {
	return (onePoint[0] == otherPoint[0]) && (onePoint[1] == otherPoint[1]) && (onePoint[2] == otherPoint[2]) && (onePoint[3] == otherPoint[3]);
}

inline bool operator!=(Matrix44 onePoint, Matrix44 otherPoint) {
	return (onePoint[0] != otherPoint[0]) && (onePoint[1] != otherPoint[1]) && (onePoint[2] != otherPoint[2]) && (onePoint[3] != otherPoint[3]);
}

inline Matrix44 operator-(Matrix44& onePoint) {
	return Matrix44(-onePoint[0], -onePoint[1], -onePoint[2], -onePoint[3]);
}

inline Matrix44 operator+(Matrix44& onePoint) {
	return Matrix44(-onePoint[0], -onePoint[1], -onePoint[2], -onePoint[3]);
}