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
	double getArea();

	void setPosition(vector2d nPosition);
	void setVelocity(vector2d nVelocity);
	void setAcceleration(vector2d nAcceleration);
	void setRadius(double nRadius);

	void addPosition(vector2d nPosition);
	void addVelocity(vector2d nVelocity);
	void addAcceleration(vector2d nAcceleration);
	void addRadius(double nRadius);

	vector2d getPosition();
	vector2d getVelocity();
	vector2d getAcceleration();
	double getRadius();

private:

	vector2d position;
	vector2d velocity;
	vector2d acceleration;

	double radius;

	std::shared_mutex mtx;
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