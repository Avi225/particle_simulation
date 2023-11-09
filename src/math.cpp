#include "math.hpp"

double mapRange(double value, double inMin, double inMax, double outMin, double outMax)
{
	return ((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
}

double aSquareRoot(const double x)  
{
  union
  {
    int i;
    double x;
  } u;

  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22); 
  return u.x;
}

double clamp(double x, double min, double max)
{
	return((x < min) ? min : (x > max) ? max : x);
}

double clamp(double x, double min)
{
	return((x < min) ? min : x);
}

double aAbs(double value)
{
	return((value<0) ? -value : value);
}

vector2d& vector2d::operator+=(vector2d const& a)
{
	this->x += a.x;
	this->y += a.y;
	return(*this);
}

vector2d& vector2d::operator-=(vector2d const& a)
{
	this->x -= a.x;
	this->y -= a.y;
	return(*this);
}

vector2d& vector2d::operator*=(vector2d const& a)
{
	this->x *= a.x;
	this->y *= a.y;
	return(*this);
}

vector2d& vector2d::operator/=(vector2d const& a)
{
	this->x /= a.x;
	this->y /= a.y;
	return(*this);
}

vector2d vector2d::operator+(vector2d const& a)
{
	vector2d b = *this;
	b.x += a.x;
	b.y += a.y;
	return(b);
}

vector2d vector2d::operator-(vector2d const& a)
{
	vector2d b = *this;
	b.x -= a.x;
	b.y -= a.y;
	return(b);
}

vector2d vector2d::operator*(vector2d const& a)
{
	vector2d b = *this;
	b.x *= a.x;
	b.y *= a.y;
	return(b);
}
vector2d vector2d::operator/(vector2d const& a)
{
	vector2d b = *this;
	b.x /= a.x;
	b.y /= a.y;
	return(b);
}

vector2d& vector2d::operator+=(double const& a)
{
	this->x += a;
	this->y += a;
	return(*this);
}

vector2d& vector2d::operator-=(double const& a)
{
	this->x -= a;
	this->y -= a;
	return(*this);
}

vector2d& vector2d::operator*=(double const& a)
{
	this->x *= a;
	this->y *= a;
	return(*this);
}

vector2d& vector2d::operator/=(double const& a)
{
	this->x /= a;
	this->y /= a;
	return(*this);
}

vector2d vector2d::operator+(double const& a)
{
	vector2d b = *this;
	b.x += a;
	b.y += a;
	return(b);
}

vector2d vector2d::operator-(double const& a)
{
	vector2d b = *this;
	b.x -= a;
	b.y -= a;
	return(b);
}

vector2d vector2d::operator*(double const& a)
{
	vector2d b = *this;
	b.x *= a;
	b.y *= a;
	return(b);
}

vector2d vector2d::operator/(double const& a)
{
	vector2d b = *this;
	b.x /= a;
	b.y /= a;
	return(b);
}

void vector2d::print()
{
	std::cout << x << ", " << y << std::endl;
}

void vector2d::normalize(double target)
{
	double i = sqrt(x * x + y * y);
	x /= i;
	y /= i;
	x *= target;
	y *= target;
}

double vector2d::distance(vector2d target)
{
	return (sqrt((x - target.x) * (x - target.x) + (y - target.y) * (y - target.y)));
}

double vector2d::dot(vector2d target)
{
	return x * target.x + y * target.y;
}


void vector2d::rotate(double degrees)
{
	double len = length();
	double radians = degrees * (3.14159265358979323846 / 180);

	double nx = cos(radians) * x - sin(radians) * y;
	double ny = sin(radians) * x + cos(radians) * y;
	x = nx;
	y = ny;
	normalize(len);
}

double vector2d::length()
{
	return(sqrt(x*x+y*y));
}

vector2d vector2d::getVector(vector2d b)
{
	return vector2d(x-b.x, y-b.y);
}

double vector2d::getAngle()
{
    return atan2(x, y)*(180/3.14159265358979323846);
}

