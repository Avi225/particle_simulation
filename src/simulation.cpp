#include "simulation.hpp"


// Constructor for the simulation container
simulationContainer::simulationContainer()
{
	density = 1; // Universal density of each particle

	restitution = 0.7; // Restitution coefficient for collisions (0: perfectly inelastic, 1: perfectly elastic)

	energyLoss = 0; // Energy loss factor during collisions

	friction = 0.001; // Friction coefficient applied to particles

	overlapGap = 0.01; // Small gap to resolve overlaps

	running = true; // Flag indicating if simulation is running

	quadrantCapacity = 192; // Maximum capacity of particles per quadrant in quadtree

	nodeHalfDimension = 5000; // Half dimension of the simulation space

	nodeQuadTree = new quadTree({vector2d(0, 0), nodeHalfDimension}, quadrantCapacity);	// Root of the quadtree system

	isPlacingParticle = false; // Flag for placing a particle

	iterationSteps = 32; // Number of iteration steps for collision resolution
}



// Update the simulation state
void simulationContainer::update()
{
    selectedQuadTree = nullptr;

    particleCount = int(particles.size());

    // Update particles' positions and velocities
    for (auto& p : particles)
    {
        // Calculate friction forces
        double frictionX = (p.velocity.x > 0) ? -friction : friction;
        double frictionY = (p.velocity.y > 0) ? -friction : friction;

        // Update velocities with accelerations
        p.velocity.x += p.acceleration.x;
        p.velocity.y += p.acceleration.y;

        // Update positions based on velocities
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;

        // Apply friction force
        p.velocity.x += frictionX;
        p.velocity.y += frictionY;
    }

    nodeQuadTree->clear();

    nodeQuadTree = new quadTree({vector2d(0, 0), nodeHalfDimension}, quadrantCapacity);

    // Insert particles into the quadtree
    for (size_t i = 0; i < particles.size(); ++i)
    {
        // Remove particles outside the nodeHalfDimension bounds
        if (particles[i].position.x > nodeHalfDimension || particles[i].position.x < -nodeHalfDimension || particles[i].position.y > nodeHalfDimension || particles[i].position.y < -nodeHalfDimension)
        {
            particles.erase(particles.begin() + i);
            continue;
        }

        nodeQuadTree->insertParticle(&particles[i]);

        particles[i].updated = false;
    }

    nodeQuadTree->split();

    // Get leaves of the quadtree
    std::vector<quadTree*> quads;
    nodeQuadTree->getLeaves(quads);

    // Create worker threads to handle collisions in each quadrant
    std::vector<std::thread> workers;
    for (auto& q : quads)
    {
        // Skip empty quadrants
        if (q->particles.size() == 0)
            continue;

        workers.push_back(std::thread(&simulationContainer::worker, this, q));
    }

    // Wait for all worker threads to finish
    for (auto& w : workers)
    {
        w.join();
    }
}


// Render the simulation
void simulationContainer::render(aCamera *camera)
{
    // Render placing particle preview line if necessary
    if(isPlacingParticle)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        vector2d mouse = {double(x), double(y)};

        mouse = camera->screenToWorld(mouse);

        camera->renderLine(placeParticlePosition, mouse, 0.1, {255, 0, 0, 255}, false);
    }

    // Render particles 
    for (auto& p : particles)
    {
        p.render(camera);
    }

    // Render debug information for particles in the selected quadtree
    if(selectedQuadTree != nullptr)
    {
        for(auto& p : selectedQuadTree->particles)
        {
            p->renderDebug(camera);
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
    int x, y;
    SDL_GetMouseState(&x, &y);
    vector2d mouse = {double(x), double(y)};
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

    nodeQuadTree->render(camera, mouse);
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
        particles.push_back(particle(placeParticlePosition, radius));
        
        // Set the velocity of the newly added particle towards the release position of the mouse
        particles[particles.size() - 1].velocity = placeParticlePosition.getVector(position);
        particles[particles.size() - 1].velocity.x /= -10;
        particles[particles.size() - 1].velocity.y /= -10;
        
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
    particles.push_back(particle(position, radius));
}

// Get a pointer to a particle by its ID
particle* simulationContainer::getParticle(int id)
{
    return &particles[id];
}

// Get the count of particles in the simulation
int* simulationContainer::getParticleCount()
{
    return &particleCount;
}

// Worker function for multithreaded collision detection
void simulationContainer::worker(quadTree* q)
{
	double radiiSum = 0;
    double overlapDistance = 0;
    vector2d overlap = {0, 0};
    double totalArea = 0;
    double factorA = 0;
    double factorB = 0;
    double areaA = 0;
    double areaB = 0;
    vector2d collisionNormal = {0, 0};
    vector2d velocity = {0, 0};
    double relativeVelocity = 0;
    double impulse = 0;
    int iterations = (int(q -> particles.size()) < iterationSteps) ? int(q -> particles.size()) < iterationSteps : iterationSteps;

    for (int ii = 0; ii < iterations; ++ii)
    {
        // Collision resolution for particles
        
        // Iterate over each particle in the current quad tree node
		for (auto& a : q->particles)
		{
		    // Lock the particle to prevent concurrent access
		    a->lock();

		    // Iterate over each particle in the current quad tree node (nested loop)
		    for (auto& b : q->particles)
		    {
		        // Avoid collision checks with the same particle
		        if (a == b)
		        	continue;
		        
	            // Calculate the sum of radii of the two particles
	            radiiSum = a->radius + b->radius;

	            // Skip collision detection if particles are not close enough
	            if ((radiiSum * radiiSum) < a->position.distanceSquared(b->position))
	            	continue;
	            
                // Calculate the distance by which the particles overlap
                overlapDistance = radiiSum - a->position.distance(b->position);
                
                // Calculate the direction vector of overlap
                overlap = a->position.getVector(b->position);

                overlap.normalize(overlapDistance);


                // Calculate the areas of the particles
                areaA = a->getArea();
                areaB = b->getArea();

                // Calculate the total area for overlap adjustment
                totalArea = areaA + areaB;

                // Calculate factors for adjusting particle positions
                factorA = areaA / totalArea;
                factorB = areaB / totalArea;

                // Adjust particle positions to resolve overlap
                a->position.x += overlap.x * factorB;
                a->position.y += overlap.y * factorB;
                b->position.x -= overlap.x * factorA;
                b->position.y -= overlap.y * factorA;

                // Calculate the collision normal direction
                collisionNormal = a->position.getVector(b->position);
                collisionNormal.normalize(1);

                // Calculate the relative velocity along the collision normal
                velocity = {b->velocity.x - a->velocity.x, b->velocity.y - a->velocity.y};
                relativeVelocity = velocity.dot(collisionNormal);

                // Calculate the collision impulse for energy exchange
                impulse = -((1 + restitution) * relativeVelocity) / (1 / (areaA * density) + 1 / (areaB * density));

                // Update velocities based on collision impulse and energy loss
                a->velocity.x -= (impulse / (areaA * density) * collisionNormal.x * (1 - energyLoss));
                a->velocity.y -= (impulse / (areaA * density) * collisionNormal.y * (1 - energyLoss));
                b->velocity.x += (impulse / (areaB * density) * collisionNormal.x * (1 - energyLoss));
                b->velocity.y += (impulse / (areaB * density) * collisionNormal.y * (1 - energyLoss));
		            
		        
		    }

		    // Unlock the particle after collision detection and resolution
		    a->unlock();
		}


        // Collision resolution with static lines
		for (auto& p : q->particles)
		{
		    // Lock the particle to prevent concurrent access
		    p->lock();

		    // Iterate over each static line for collision detection
		    for (auto& staticLine : staticLines)
		    {
		        // Check for overlap between the particle and the static line
		        double overlap = staticLine.checkParticleCollision(*p);
		        
		        // If overlap exists
		        if (overlap < p->radius)
		        {
		            // Get the normal vector of the static line
		            vector2d normal = staticLine.getNormal();
		            normal.normalize(1);

		            // Adjust particle positions to resolve overlap
		            p->position.x += (p->radius - overlap + overlapGap) * normal.x;
		            p->position.y += (p->radius - overlap + overlapGap) * normal.y;

		            // Calculate the dot product of particle velocity and normal vector
		            double dot = 2 * (p->velocity.x * normal.x + p->velocity.y * normal.y);

		            // Reflect particle velocity based on collision normal, restitution factor, and energy loss
		            p->velocity.x -= dot * normal.x * (1 - energyLoss) * restitution;
		            p->velocity.y -= dot * normal.y * (1 - energyLoss) * restitution;
		        }
		    }

		    // Unlock the particle after collision resolution
		    p->unlock();
		}
    }
}

