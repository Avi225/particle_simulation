#pragma once

#include <vector>
#include <cstdio>
#include <bits/stdc++.h>

#include "math.hpp"
#include "aCamera.hpp"


struct particle
{
	particle(vector2f nPosition, float nRadius);
	void render(aCamera *camera);
	void renderVector(aCamera *camera);
	bool checkCollision(particle b);
	float getArea();

	vector2f previousPosition;
	vector2f position;
	vector2f velocity;
	vector2f acceleration;

	float radius;
};

struct staticPoint
{
	staticPoint(vector2f nPosition);
	vector2f position;
};

struct staticLine
{
	staticLine(staticPoint *na, staticPoint *nb);
	void render(aCamera *camera);
	void renderNormal(aCamera *camera);
	vector2f getNormal();

	float checkParticleCollision(particle target);
	staticPoint* a;
	staticPoint* b;
};

class simulationContainer
{
	public:
		simulationContainer(aWindow* window);

		void update();
		void render(aCamera *camera);
		void placeParticle(vector2f position, float radius, bool state);

		void switchRunning();
		bool getRunning();

		void addParticle(vector2f position, float radius);
		particle* getParticle(int id);

	private:
		aWindow* window;
		bool isPlacingParticle;
		vector2f placeParticlePosition;
		bool running;
		std::vector<particle> particles;
		std::vector<staticPoint> staticPoints;
		std::vector<staticLine> staticLines;

		float density;
		float restitution;
		float energyLoss;

		int iterationSteps;
};