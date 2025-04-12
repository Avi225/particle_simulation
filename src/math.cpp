#include "math.hpp"

#include <cstring>

#include "log.hpp"

double mapRange(double value, double inMin, double inMax, double outMin, double outMax)
{
	return ((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
}

double aSquareRoot(const double x)  
{
    double x0 = x / 2.0; // Initial guess

    for (int i = 0; i < 10; ++i) {
        x0 = (x0 + x / x0) / 2.0; // Newton-Raphson iteration
    }

    return x0;;
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

double isqrt(double target)
{
    const float th = 1.5F;

    float x2 = static_cast<float>(target) * 0.5F;
    float y = static_cast<float>(target);

    unsigned int i;
    memcpy(&i, &y, sizeof(i));

    i = 0x5f3759df - (i >> 1);
    memcpy(&y, &i, sizeof(i));

    y = y * (th - (x2 * y * y));
    y = y * (th - (x2 * y * y));

    return static_cast<double>(y);
}

SDL_Color interpolateColor(const SDL_Color& color1, const SDL_Color& color2, double factor)
{
    factor = std::max(0.0, std::min(1.0, factor));
    SDL_Color result;
    result.r = static_cast<Uint8>(color1.r + (color2.r - color1.r) * factor);
    result.g = static_cast<Uint8>(color1.g + (color2.g - color1.g) * factor);
    result.b = static_cast<Uint8>(color1.b + (color2.b - color1.b) * factor);
    result.a = static_cast<Uint8>(color1.a + (color2.a - color1.a) * factor);
    return result;
}

double interpolateDouble(double value1, double value2, double factor)
{
    factor = std::max(0.0, std::min(1.0, factor));
    return value1 + (value2 - value1) * factor;
}

vector2d::vector2d()
: x(0), y(0)
{}

vector2d::vector2d(double x, double y)
: x(x), y(y)
{}

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
	log::info("{}, {}", x, y);
}

void vector2d::normalize(double target)
{
	double i = isqrt(x * x + y * y);
	x *= i;
	y *= i;
	x *= target;
	y *= target;
}

void vector2d::normalizeSquared(double target)
{
	double i = x * x + y * y;
	x /= i;
	y /= i;
	x *= target;
	y *= target;
}

double vector2d::distance(vector2d target)
{
	return 1 / isqrt((x - target.x) * (x - target.x) + (y - target.y) * (y - target.y));
}

double vector2d::distanceSquared(vector2d target)
{
	return ((x - target.x) * (x - target.x) + (y - target.y) * (y - target.y));
}

double vector2d::dot(vector2d target)
{
	return x * target.x + y * target.y;
}

void vector2d::rotate(double degrees)
{
	double radians = degrees * 3.14159265358979323846 / 180.0;
    double cosA = cos(radians);
    double sinA = sin(radians);

    x = x * cosA - y * sinA;
    y = x * sinA + y * cosA;
}

double vector2d::length()
{
	return(isqrt(x*x+y*y));
}

vector2d vector2d::getVector(vector2d b)
{
	return vector2d(x-b.x, y-b.y);
}

double vector2d::getAngle()
{
    return atan2(x, y)*(180/3.14159265358979323846);
}
