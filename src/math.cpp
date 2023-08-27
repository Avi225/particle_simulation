#include "math.hpp"

float mapRange(float value, float inMin, float inMax, float outMin, float outMax)
{
	return ((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
}

float aSquareRoot(const float x)  
{
  union
  {
    int i;
    float x;
  } u;

  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22); 
  return u.x;
}

float clamp(float x, float min, float max)
{
	return((x < min) ? min : (x > max) ? max : x);
}

float clamp(float x, float min)
{
	return((x < min) ? min : x);
}

float aAbs(float value)
{
	return((value<0) ? -value : value);
}


void vector2f::print()
{
	std::cout << x << ", " << y << std::endl;
}

void vector2f::normalize(float target)
{
	float i = aSquareRoot(x * x + y * y);
	x /= i;
	y /= i;
	x *= target;
	y *= target;
}

float vector2f::distance(vector2f target)
{
	return (sqrt((x - target.x) * (x - target.x) + (y - target.y) * (y - target.y)));
}

float vector2f::dot(vector2f target)
{
	return x * target.x + y * target.y;
}


void vector2f::rotate(float degrees)
{
	float len = length();
	float radians = degrees * 3.14159265358979323846 / 180;

	float nx = cos(radians) * x - sin(radians) * y;
	float ny = sin(radians) * x + cos(radians) * y;
	x = nx;
	y = ny;
	normalize(len);
}

float vector2f::length()
{
	return(aSquareRoot(x*x+y*y));
}

vector2f vector2f::getVector(vector2f b)
{
	return vector2f(x-b.x, y-b.y);
}

float vector2f::getAngle()
{
    return atan2(x, y)*(180/3.14159265358979323846);
}

