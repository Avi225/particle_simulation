#pragma once

#include <vector>
#include <cstdio>
#include <bits/stdc++.h>

#include "math.hpp"
#include "aCamera.hpp"


struct particle
{
	particle(vector2d nPosition, double nRadius);
	void render(aCamera *camera);
	void renderVector(aCamera *camera);
	bool checkCollision(particle b);
	double getArea();

	vector2d previousPosition;
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

	double checkParticleCollision(particle target);
	staticPoint* a;
	staticPoint* b;
};

class simulationContainer
{
	public:
		simulationContainer();

		void update();
		void render(aCamera *camera);
		void placeParticle(vector2d position, double radius, bool state);
		
		void addStaticPoint(vector2d position);
		void addStaticLine(int a, int b);

		void switchRunning();
		bool getRunning();

		void addParticle(vector2d position, double radius);
		particle* getParticle(int id);

	private:
		bool isPlacingParticle;
		vector2d placeParticlePosition;
		bool running;
		std::vector<particle> particles;
		std::vector<staticPoint> staticPoints;
		std::vector<staticLine> staticLines;

		double density;
		double restitution;
		double energyLoss;

		int iterationSteps;
};