#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cmath>
#include <iostream>

double mapRange(double value, double inMin, double inMax, double outMin, double outMax);

double aSquareRoot(const double x);

double clamp(double x, double min, double max);

double clamp(double x, double min);

double aAbs(double value);

double isqrt(double target);

SDL_Color interpolateColor(const SDL_Color& color1, const SDL_Color& color2, double factor);

double interpolateDouble(double value1, double value2, double factor);

struct vector2d
{
	vector2d();

	vector2d(double x, double y);

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
	void normalizeSquared(double target);

	double distance(vector2d target);

	double distanceSquared(vector2d target);

	double dot(vector2d target);

	void rotate(double degrees);

	double length();

	vector2d getVector(vector2d b);

	double getAngle();
	
	double x, y;
};
