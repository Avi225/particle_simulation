#include "simulation.hpp"

// Constructor for the simulation container
simulationContainer::simulationContainer()
{
	pool = new ThreadPool(12);

	density = 1; // Universal density of each particle

	restitution = 0.7; // Restitution coefficient for collisions (0: perfectly inelastic, 1: perfectly elastic)

	energyLoss = 0; // Energy loss factor during collisions

	friction = 0.001; // Friction coefficient applied to particles

	overlapGap = 0.01; // Small gap to resolve overlaps

	running = false; // Flag indicating if simulation is running

	quadrantCapacity = 192/2; // Maximum capacity of particles per quadrant in quadtree

	nodeHalfDimension = 8192; // Half dimension of the simulation space

	nodeQuadTree = new quadTree({vector2d(0, 0), nodeHalfDimension}, quadrantCapacity);	// Root of the quadtree system

	isPlacingParticle = false; // Flag for placing a particle

	placeParticlePosition = vector2d(0, 0); // Position of placed particle

	running = false; // Flag to determine if the simulation is being updated

	particleCount = 0; // Total number of particles currently being simulated

	selectedQuadTree = nullptr; // Currently selected quadrant of the QuadTree, for debugging

	iterationSteps = 2; // Number of iteration steps for collision resolution

	log::info("simulationContainer::simulationContainer - Constructor finished successfully.");
}

void simulationContainer::cleanUp()
{
	delete pool;
	nodeQuadTree -> clear();
	delete nodeQuadTree;
	for (particle* p : particles) {
		delete p;
	}
	particles.clear();
}

// Update the simulation state
void simulationContainer::update()
{
	// Update particles' positions and velocities
	for (auto& p : particles)
	{
		// Calculate friction forces
		double frictionX = (p -> getVelocity().x > 0) ? -friction : friction;
		double frictionY = (p -> getVelocity().y > 0) ? -friction : friction;

		// Update velocities with accelerations
		p -> setVelocity(p -> getVelocity() + p -> getAcceleration());

		// Update positions based on velocities
		p -> setPosition(p -> getPosition() + p -> getVelocity());

		// Apply friction force
		p -> setVelocity({p -> getVelocity().x + frictionX, p -> getVelocity().y + frictionY});
	}
	for (int i = 0; i < iterationSteps; ++i)
	{
		selectedQuadTree = nullptr;
		nodeQuadTree->clear();
		delete nodeQuadTree;
		nodeQuadTree = new quadTree({vector2d(0, 0), nodeHalfDimension}, quadrantCapacity);

		//Insert particles into the quadtree
		for (size_t i = particles.size(); i > 0; --i)
		{
		    size_t index = i - 1;
		    if (particles[index]->getPosition().x > nodeHalfDimension ||
		        particles[index]->getPosition().x < -nodeHalfDimension ||
		        particles[index]->getPosition().y > nodeHalfDimension ||
		        particles[index]->getPosition().y < -nodeHalfDimension)
		    {
		        delete particles[index];
		        particles.erase(particles.begin() + index);
		    }
		    else
		    {
		    	nodeQuadTree->insertParticle(particles[index]);
		    }
		}

		nodeQuadTree->split();

		std::vector<quadTree*> quads;
		nodeQuadTree->getLeaves(quads);

		std::unordered_map<int, std::future<bool>> tasks;
		
		for (size_t i = 0; i < quads.size(); ++i)
		{
			// Skip empty quadrants
			if (quads[i]->particles.size() == 0)
				continue; 

			int index = int(i);
			tasks[index] = pool->enqueue([this, &quads, index]()
			{
				worker(quads[index]);
				return true;
			});
		}

		for (auto& [key, task]: tasks)
		{
			task.get();
		}
	}
}

// Render the simulation
void simulationContainer::render(aCamera *camera)
{
	// Render placing particle preview line if necessary
	if(isPlacingParticle)
	{
		float x, y;
		SDL_GetMouseState(&x, &y);
		vector2d mouse = {x, y};

		mouse = camera->screenToWorld(mouse);

		camera->renderLine(placeParticlePosition, mouse, 0.1, {255, 0, 0, 255}, false);
	}

	// Render particles 
	for (auto& p : particles)
	{
		p -> render(camera);
	}

	//Render debug information for particles in the selected quadtree
	if(selectedQuadTree != nullptr)
	{
		 for(auto& p : selectedQuadTree->particles)
		 {
			 p -> renderDebug(camera);
		 }
	}

	// Render static lines and their normals
	for(auto& l : staticLines)
	{
		l.render(camera);
		l.renderNormal(camera);
	}
}

// Select a quadtree
void simulationContainer::select(aCamera* camera)
{
	// Retrieve all leaf nodes of the quadtree
	std::vector<quadTree*> quads;
	nodeQuadTree->getLeaves(quads);

	// Get the current mouse position
	float x, y;
	SDL_GetMouseState(&x, &y);
	vector2d mouse = {x, y};
	mouse = camera->screenToWorld(mouse);

	// Iterate through each leaf node of the quadtree
	for(auto& q : quads)
	{
		// Check if the mouse position falls within the bounds of the current quadtree node
		if(mouse.x < q->boundary.center.x + q->boundary.halfDimension &&
			mouse.x > q->boundary.center.x - q->boundary.halfDimension &&
			mouse.y < q->boundary.center.y + q->boundary.halfDimension &&
			mouse.y > q->boundary.center.y - q->boundary.halfDimension)
		{
			selectedQuadTree = q;
			break;
		}
	}
}


// Render the quadtree structure on the screen
void simulationContainer::renderQuadTree(aCamera *camera, vector2d mouse)
{
	// Return if the quadtree is not initialized
	if (nodeQuadTree == nullptr)
		return;

	nodeQuadTree -> render(camera, mouse);
}

// Start placing and place a particle at a specified position
void simulationContainer::placeParticle(vector2d position, double radius, bool state)
{
	if (!state)
	{
		// Set the position for placing a particle and activate placement mode
		placeParticlePosition = position;
		isPlacingParticle = true;
	}
	else
	{
		// Add a new particle to the simulation at the position where the mouse was released
		particles.push_back(new particle(placeParticlePosition, radius));
		
		// Set the velocity of the newly added particle towards the release position of the mouse
		particles[particles.size() - 1] -> setVelocity(placeParticlePosition.getVector(position));
		particles[particles.size() - 1] -> setVelocity(particles[particles.size() - 1] -> getVelocity() / -10);
		
		// Deactivate placement mode
		isPlacingParticle = false;
	}
}

// Add a static point to the simulation
void simulationContainer::addStaticPoint(vector2d position)
{
	staticPoints.push_back(staticPoint(position));
}

// Add a static line to the simulation
void simulationContainer::addStaticLine(int a, int b)
{
	staticLines.push_back(staticLine(&staticPoints[a], &staticPoints[b]));
}

// Toggle the simulation running state
void simulationContainer::switchRunning()
{
	running = !running;
}

// Get the simulation running state
bool simulationContainer::getRunning()
{
	return running;
}

// Add a particle to the simulation
void simulationContainer::addParticle(vector2d position, double radius)
{
	particles.push_back(new particle(position, radius));
}

// Get a pointer to a particle by its ID
particle* simulationContainer::getParticle(int id)
{
	return particles[id];
}

// Get the count of particles in the simulation
int simulationContainer::getParticleCount()
{
	return int(particles.size());
}

void simulationContainer::worker(quadTree* q)
{
	for (particle* a : q->particles)
	{
		vector2d positionA = a->getPosition();
		vector2d velocityA = a->getVelocity();
		double radiusA = a->getRadius();
		double areaA = a->getArea();

		for (particle* b : q->particles)
		{
			if (a == b) // Skip collision checks with the same particle
				continue;

			vector2d positionB = b->getPosition();

			double radiiSum = radiusA + b->getRadius();

			if (std::abs(positionA.x - positionB.x) >= radiiSum ||
				std::abs(positionA.y - positionB.y) >= radiiSum) // Skip collision detection if particles are not close enough
				continue;

			double distanceSquared = positionA.distanceSquared(positionB);
			double radiiSumSquared = radiiSum * radiiSum;
			if (distanceSquared > radiiSumSquared) // Skip collision detection if particles are not close enough
				continue;

			double overlapDistance = radiiSum - std::sqrt(distanceSquared);
			if (overlapDistance <= 0)
				continue;

			vector2d overlap = positionA.getVector(positionB);
			overlap.normalize(overlapDistance);

			double areaB = b->getArea();

			double totalArea = areaA + areaB;
			double factorA = areaA / totalArea;
			double factorB = areaB / totalArea;

			positionA.x += overlap.x * factorB;
			positionA.y += overlap.y * factorB;
			
			positionB.x -= overlap.x * factorA;
			positionB.y -= overlap.y * factorA;

			vector2d collisionNormal = positionA.getVector(positionB);
			collisionNormal.normalize(1);

			vector2d velocity = b->getVelocity() - velocityA;
			double relativeVelocity = velocity.dot(collisionNormal);

			double impulse = -((1 + restitution) * relativeVelocity) / (1 / (areaA * density) + 1 / (areaB * density));

			velocityA.x -= (impulse / (areaA * density)) * collisionNormal.x * (1 - energyLoss);
			velocityA.y -= (impulse / (areaA * density)) * collisionNormal.y * (1 - energyLoss);
			
			b->setVelocity({b->getVelocity().x + (impulse / (areaB * density)) * collisionNormal.x * (1 - energyLoss),
						   b->getVelocity().y + (impulse / (areaB * density)) * collisionNormal.y * (1 - energyLoss)});

			b->setPosition(positionB);
		}

		a->setPosition(positionA);
		a->setVelocity(velocityA);
	}
}
