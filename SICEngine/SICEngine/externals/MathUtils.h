/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability 
* of this software for any purpose.  
* It is provided "as is" without express or implied warranty.
*/

// Source: https://github.com/erincatto/box2d-lite.

// Modifications to the original software (Box2D Lite):
/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "../SICTypes.h"

#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>

const float k_pi = 3.14159265358979323846264f;


struct Mat22
{
	Mat22() {}
	Mat22(float angle)
	{
		float c = cosf(angle), s = sinf(angle);
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	Mat22(const sic::vec2& col1, const sic::vec2& col2) : col1(col1), col2(col2) {}

	Mat22 Transpose() const
	{
		return Mat22(sic::vec2(col1.x, col2.x), sic::vec2(col1.y, col2.y));
	}

	Mat22 Invert() const
	{
		float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		Mat22 B;
		float det = a * d - b * c;
		assert(det != 0.0f);
		det = 1.0f / det;
		B.col1.x =  det * d;	B.col2.x = -det * b;
		B.col1.y = -det * c;	B.col2.y =  det * a;
		return B;
	}

	sic::vec2 col1, col2;
};

inline float Dot(const sic::vec2& a, const sic::vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float Cross(const sic::vec2& a, const sic::vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

inline sic::vec2 Cross(const sic::vec2& a, float s)
{
	return sic::vec2(s * a.y, -s * a.x);
}

inline sic::vec2 Cross(float s, const sic::vec2& a)
{
	return sic::vec2(-s * a.y, s * a.x);
}

inline sic::vec2 operator * (const Mat22& A, const sic::vec2& v)
{
	return sic::vec2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

inline sic::vec2 operator + (const sic::vec2& a, const sic::vec2& b)
{
	return sic::vec2(a.x + b.x, a.y + b.y);
}

inline sic::vec2 operator - (const sic::vec2& a, const sic::vec2& b)
{
	return sic::vec2(a.x - b.x, a.y - b.y);
}

inline sic::vec2 operator * (float s, const sic::vec2& v)
{
	return sic::vec2(s * v.x, s * v.y);
}

inline Mat22 operator + (const Mat22& A, const Mat22& B)
{
	return Mat22(A.col1 + B.col1, A.col2 + B.col2);
}

inline Mat22 operator * (const Mat22& A, const Mat22& B)
{
	return Mat22(A * B.col1, A * B.col2);
}

inline float Abs(float a)
{
	return a > 0.0f ? a : -a;
}

inline sic::vec2 Abs(const sic::vec2& a)
{
	return sic::vec2(fabsf(a.x), fabsf(a.y));
}

inline Mat22 Abs(const Mat22& A)
{
	return Mat22(Abs(A.col1), Abs(A.col2));
}

inline float Sign(float x)
{
	return x < 0.0f ? -1.0f : 1.0f;
}

inline float Min(float a, float b)
{
	return a < b ? a : b;
}

inline float Max(float a, float b)
{
	return a > b ? a : b;
}

inline float Clamp(float a, float low, float high)
{
	return Max(low, Min(a, high));
}

template<typename T> inline void Swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

// Random number in range [-1,1]
inline float Random()
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

inline float Random(float lo, float hi)
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}

#endif

