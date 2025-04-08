#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>

#include "math.hpp"
#include "utility.hpp"
#include "aCamera.hpp"

class particle
{
public:

	particle();
	particle(vector2d nPosition, double nRadius);

	void render(aCamera *camera);
	void renderDebug(aCamera *camera);
	
	inline double getArea() {return 3.14159265359 * radius * radius;}

	inline void setPosition(vector2d nPosition) {position = nPosition;}
	inline void setVelocity(vector2d nVelocity) {velocity = nVelocity;}
	inline void setAcceleration(vector2d nAcceleration) {acceleration = nAcceleration;}
	inline void setRadius(double nRadius) {radius = nRadius;}

	inline vector2d getPosition() {return position;}
	inline vector2d getVelocity() {return velocity;}
	inline vector2d getAcceleration() {return acceleration;}
	inline double getRadius() {return radius;}

private:

	vector2d position;
	vector2d velocity;
	vector2d acceleration;

	double radius;
};

struct staticPoint
{
	staticPoint(vector2d nPosition);
	vector2d position;
};

struct staticLine
{
	staticLine(staticPoint *na, staticPoint *nb);
	void render(aCamera *camera);
	void renderNormal(aCamera *camera);
	vector2d getNormal();
	
	staticPoint* a;
	staticPoint* b;
};