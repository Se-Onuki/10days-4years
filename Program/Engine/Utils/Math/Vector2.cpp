/// @file Vector2.cpp
/// @brief Vector2
/// @author ONUKI seiya
#include "Vector2.h"

//#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>


#include"Math.hpp"
#include "Matrix2x2.h"
#include "Matrix3x3.h"

#include <assert.h>

namespace SoLib {

	const Vector2 Vector2::zero = { 0.f,0.f };
	const Vector2 Vector2::right = { 1.f,0.f };
	const Vector2 Vector2::up = { 0.f,1.f };
	const Vector2 Vector2::one = { 1.f,1.f };


	void Vector2::Reset() {
		*this = ZeroVector2;
	}

	Vector2 Vector2::Rotate(const float &theta) const {
		return *this * MakeRotateMatrix(theta);
	}

	float Vector2::LengthSQ() const {
		return x * x + y * y;
	}

	float Vector2::Length() const {
		return sqrtf(LengthSQ());
	}

	Vector2 Vector2::Normalize() const {
		float Length = this->Length();
		if (Length != 0) {
			return *this / Length;
		}
		else {
			return ZeroVector2;
		}
	}
	//
	//void Vector2::Printf(int x_, int y_) const {
	////	Novice::ScreenPrintf(x_, y_, "%12.2f // %6.2f", this->x, Length());
	////	Novice::ScreenPrintf(x_, y_ + TextHeight, "%12.2f", this->y);
	//}

	Vector2 Vector2::operator+(const Vector2 Second) const {
		return { (this->x) + (Second.x), (this->y) + (Second.y) };
	}

	Vector2 Vector2::operator-(const Vector2 Second) const {
		return { (this->x) - (Second.x), (this->y) - (Second.y) };
	}

	void Vector2::operator +=(const Vector2 Second) {
		this->x += Second.x;
		this->y += Second.y;
	}

	void Vector2::operator -=(const Vector2 Second) {
		this->x -= Second.x;
		this->y -= Second.y;
	}

	Vector2 Vector2::operator*(const float &Second) const {
		return { (this->x) * (Second), (this->y) * (Second) };
	}

	Vector2 Vector2::operator/(const float &Second) const {
		return { (this->x) / (Second), (this->y) / (Second) };
	}

	void Vector2::operator *=(const float &Second) {
		this->x *= Second;
		this->y *= Second;
	}


	void Vector2::operator /=(const float &Second) {
		this->x /= Second;
		this->y /= Second;
	}

	Vector2 Vector2::operator*(const Matrix2x2 &Second) const {
		const Vector2 b[2]{ {Second.m[0][0],Second.m[1][0]},{Second.m[0][1],Second.m[1][1]} };
		return Vector2{
			(*this * b[0]),
			(*this * b[1])
		};
	}

	void Vector2::operator*=(const Matrix2x2 &Second) {
		const Vector2 b[2]{ {Second.m[0][0],Second.m[1][0]},{Second.m[0][1],Second.m[1][1]} };
		*this = {
			(*this * b[0]),
			(*this * b[1])
		};
	}

	Vector2 Vector2::operator*(const Matrix3x3 &matrix) const {
		Vector2 result;

		result.x = x * matrix.m[0][0] + y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
		result.y = x * matrix.m[0][1] + y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
		const float w = x * matrix.m[0][2] + y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
		assert(w != 0.0f);
		return result / w; // 演算子のオーバーライド
	}

	void Vector2::operator*=(const Matrix3x3 &matrix) {
		this->x = x * matrix.m[0][0] + y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
		this->y = x * matrix.m[0][1] + y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
		const float w = x * matrix.m[0][2] + y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
		assert(w != 0.0f);
		*this / w; // 演算子のオーバーライド
	}


}