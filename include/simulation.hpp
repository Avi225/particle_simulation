#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <memory>


#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include "math.hpp"
#include "aCamera.hpp"

struct MutexWrapper
{
    std::unique_ptr<std::mutex> mutex_ptr;

    MutexWrapper() : mutex_ptr(std::make_unique<std::mutex>()) {}

    MutexWrapper(MutexWrapper&& other) noexcept : mutex_ptr(std::move(other.mutex_ptr)) {}

    MutexWrapper& operator=(MutexWrapper&& other) noexcept {
        if (this != &other)
        {
            mutex_ptr = std::move(other.mutex_ptr);
        }
        return *this;
    }
};

struct particle
{
	particle();
	particle(vector2d nPosition, double nRadius);
	void render(aCamera *camera);
	void renderVector(aCamera *camera);
	bool checkCollision(particle b);
	double getArea();

	void lock();
	void unlock();

	particle(particle&& other) noexcept
        : position(std::move(other.position)),
          velocity(std::move(other.velocity)),
          acceleration(std::move(other.acceleration)),
          radius(other.radius),
          mutex_(std::move(other.mutex_)) {}

    particle& operator=(particle&& other) noexcept
    {
        if (this != &other)
        {
            position = std::move(other.position);
            velocity = std::move(other.velocity);
            acceleration = std::move(other.acceleration);
            radius = other.radius;
            // Move the mutex wrapper
            mutex_ = std::move(other.mutex_);
        }
        return *this;
    }

	vector2d previousPosition;
	vector2d position;
	vector2d velocity;
	vector2d acceleration;

	double radius;

	MutexWrapper mutex_;

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

struct quadTreeBox
{
	quadTreeBox();
	quadTreeBox(vector2d center, double halfDimension);

	vector2d center;
	double halfDimension;
};

struct quadTree
{

	quadTree(quadTreeBox nBoundary, int nCapacity);

	void split();
	void render(aCamera* camera);
	void insertParticle(particle* p);
	void clear();
	void getLeaves(std::vector<quadTree*>& quads);

	quadTreeBox boundary;
	const int capacity;
	std::vector<particle*> particles;

	quadTree* nw;
	quadTree* ne;
	quadTree* sw;
	quadTree* se;

};

class simulationContainer
{
	public:
		simulationContainer();

		void update();
		void render(aCamera *camera);
		void renderQuadTree(aCamera *camera);
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

		std::vector<particle> particles;
		std::vector<staticPoint> staticPoints;
		std::vector<staticLine> staticLines;

		quadTree* nodeQuadTree;

		double density;
		double restitution;
		double energyLoss;
		double friction;
		double overlapGap;

		int iterationSteps;
};