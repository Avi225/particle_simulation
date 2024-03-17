#pragma once

#include <cmath>
#include <iostream>

double mapRange(double value, double inMin, double inMax, double outMin, double outMax);

double aSquareRoot(const double x);

double clamp(double x, double min, double max);

double clamp(double x, double min);

double aAbs(double value);

struct vector2d
{
	vector2d() {}

	vector2d(double x, double y)
		: x(x), y(y)
	{}

	vector2d& operator+=(vector2d const& a);
	vector2d& operator-=(vector2d const& a);
	vector2d& operator*=(vector2d const& a);
	vector2d& operator/=(vector2d const& a);

	vector2d operator+(vector2d const& a);
	vector2d operator-(vector2d const& a);
	vector2d operator*(vector2d const& a);
	vector2d operator/(vector2d const& a);

	vector2d& operator+=(double const& a);
	vector2d& operator-=(double const& a);
	vector2d& operator*=(double const& a);
	vector2d& operator/=(double const& a);

	vector2d operator+(double const& a);
	vector2d operator-(double const& a);
	vector2d operator*(double const& a);
	vector2d operator/(double const& a);

	void print();

	void normalize(double target);

	double distance(vector2d target);

	double distanceSquared(vector2d target);

	double dot(vector2d target);

	void rotate(double degrees);

	double length();

	vector2d getVector(vector2d b);

	double getAngle();
	
	double x, y;
};
