#pragma once

#include <vector>
#include <string>

#include "math.hpp"
#include "utility.hpp"
#include "aCamera.hpp"

struct particle
{
	particle();
	particle(vector2d nPosition, double nRadius);
	particle(particle&& other);
    particle& operator=(particle&& other);

	void render(aCamera *camera);
	void renderDebug(aCamera *camera);
	double getArea();

	void lock();
	void unlock();

	vector2d previousPosition;
	vector2d position;
	vector2d velocity;
	vector2d acceleration;

	double radius;

	mutexWrapper mutex_;

	bool updated;
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

	double checkParticleCollision(const particle& target);
	staticPoint* a;
	staticPoint* b;
};