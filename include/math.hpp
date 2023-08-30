#pragma once

#include <cmath>
#include <iostream>

float mapRange(float value, float inMin, float inMax, float outMin, float outMax);

float aSquareRoot(const float x);

float clamp(float x, float min, float max);

float clamp(float x, float min);

float aAbs(float value);

struct vector2f
{
	vector2f() {}

	vector2f(float x, float y)
		: x(x), y(y)
	{}

	vector2f& operator+=(vector2f const& a);
	vector2f& operator-=(vector2f const& a);
	vector2f& operator*=(vector2f const& a);
	vector2f& operator/=(vector2f const& a);

	vector2f operator+(vector2f const& a);
	vector2f operator-(vector2f const& a);
	vector2f operator*(vector2f const& a);
	vector2f operator/(vector2f const& a);

	vector2f& operator+=(float const& a);
	vector2f& operator-=(float const& a);
	vector2f& operator*=(float const& a);
	vector2f& operator/=(float const& a);

	vector2f operator+(float const& a);
	vector2f operator-(float const& a);
	vector2f operator*(float const& a);
	vector2f operator/(float const& a);

	void print();

	void normalize(float target);

	float distance(vector2f target);

	float dot(vector2f target);

	void rotate(float degrees);

	float length();

	vector2f getVector(vector2f b);

	float getAngle();
	
	float x, y;
};
