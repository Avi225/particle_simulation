#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <unordered_map>

#include <ThreadPool.h>

#include "math.hpp"
#include "utility.hpp"
#include "aCamera.hpp"
#include "simulationElements.hpp"
#include "quadTree.hpp"



class simulationContainer
{
	public:
		simulationContainer();

		void update();
		void render(aCamera *camera);
		void select(aCamera* camera);
		void renderQuadTree(aCamera *camera, vector2d mouse);
		void placeParticle(vector2d position, double radius, bool state);
		
		void addStaticPoint(vector2d position);
		void addStaticLine(int a, int b);

		void switchRunning();
		bool getRunning();

		void addParticle(vector2d position, double radius);
		particle* getParticle(int id);
		int* getParticleCount();

	private:
		void worker(quadTree* q);

		bool isPlacingParticle;
		vector2d placeParticlePosition;

		bool running;

		int quadrantCapacity;
		int particleCount;

		std::vector<particle*> particles;
		std::vector<staticPoint> staticPoints;
		std::vector<staticLine> staticLines;

		ThreadPool pool;

		double nodeHalfDimension;
		quadTree* nodeQuadTree;
		quadTree* selectedQuadTree;

		double density;
		double restitution;
		double energyLoss;
		double friction;
		double overlapGap;

		int iterationSteps;
};