#include "simulation.hpp"


// Constructor for the simulation container
simulationContainer::simulationContainer()
:pool(12)
{
	density = 1; // Universal density of each particle

	restitution = 0.7; // Restitution coefficient for collisions (0: perfectly inelastic, 1: perfectly elastic)

	energyLoss = 0; // Energy loss factor during collisions

	friction = 0.001; // Friction coefficient applied to particles

	overlapGap = 0.01; // Small gap to resolve overlaps

	running = true; // Flag indicating if simulation is running

	quadrantCapacity = 192/2; // Maximum capacity of particles per quadrant in quadtree

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
        double frictionX = (p -> getVelocity().x > 0) ? -friction : friction;
        double frictionY = (p -> getVelocity().y > 0) ? -friction : friction;

        // Update velocities with accelerations
        p -> setVelocity(p -> getVelocity() + p -> getAcceleration());

        // Update positions based on velocities
        p -> setPosition(p -> getPosition() + p -> getVelocity());

        // Apply friction force
        p -> setVelocity({p -> getVelocity().x + frictionX, p -> getVelocity().y + frictionY});
    }

    nodeQuadTree->clear();

    nodeQuadTree = new quadTree({vector2d(0, 0), nodeHalfDimension}, quadrantCapacity);

    // Insert particles into the quadtree
    for (size_t i = 0; i < particles.size(); ++i)
    {
        // Remove particles outside the nodeHalfDimension bounds
        if (particles[i] -> getPosition().x > nodeHalfDimension || particles[i] -> getPosition().x < -nodeHalfDimension || particles[i] -> getPosition().y > nodeHalfDimension || particles[i] -> getPosition().y < -nodeHalfDimension)
        {
        	delete particles[i];
            particles.erase(particles.begin() + i);
            continue;
        }

        nodeQuadTree->insertParticle(particles[i]);
    }

    nodeQuadTree->split();

    // Get leaves of the quadtree
    std::vector<quadTree*> quads;
    nodeQuadTree->getLeaves(quads);

    std::unordered_map<int, std::future<bool>> tasks;
	

    for (size_t i = 0; i < quads.size(); ++i)
    {
        // Skip empty quadrants
        if (quads[i]->particles.size() == 0)
            continue; 
 
        tasks[i] = pool.enqueue([this, &quads, i](){ 
	        worker(quads[i]);
	        return true;
    	});   
        //workers.push_back(std::thread(&simulationContainer::worker, this, q));
    }

    for (auto& [key, task]: tasks)
    {
    	task.get();
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
        p -> render(camera);
    }

    // Render debug information for particles in the selected quadtree
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
int* simulationContainer::getParticleCount()
{
    return &particleCount;
}

void simulationContainer::worker(quadTree* q)
{
    for (int ii = 0; ii < iterationSteps; ++ii)
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
                double radiusB = b->getRadius();
                double areaB = b->getArea();

                double radiiSum = radiusA + radiusB;

                if (abs(positionA.x - positionB.x) >= radiiSum ||
                    abs(positionA.y - positionB.y) >= radiiSum) // Skip collision detection if particles are not close enough
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
}


// Worker function for multithreaded collision detection
// void simulationContainer::worker(quadTree* q)
// {
// 	double radiiSum = 0;
//     double overlapDistance = 0;
//     vector2d overlap = {0, 0};
//     double totalArea = 0;
//     double factorA = 0;
//     double factorB = 0;
//     double areaA = 0;
//     double areaB = 0;
//     double radiusA = 0;
//     double radiusB = 0;



//     vector2d positionA = {0, 0};
//     vector2d positionB = {0, 0};

//     vector2d velocityA = {0, 0};
//     vector2d velocityB = {0, 0};

//     vector2d collisionNormal = {0, 0};
//     vector2d velocity = {0, 0};
//     double relativeVelocity = 0;
//     double impulse = 0;
   
//     for (int ii = 0; ii < iterationSteps; ++ii)
//     {
//     // Collision resolution for particles
        
//     // Iterate over each particle in the current quad tree node	
// 		for (particle* a : q->particles)
// 		{
//     	positionA = a -> getPosition();
//     	velocityA = a -> getVelocity();
//     	radiusA = a -> getRadius();
//     	areaA = a -> getArea();

// 		// Iterate over each particle in the current quad tree node (nested loop)
// 			for (particle* b : q->particles)
// 			{
//      			positionB = b -> getPosition();
//      			radiusB = b -> getRadius();
	            
// 	            // Calculate the sum of radii of the two particles
// 	            radiiSum = radiusA + radiusB;

// 	            //Skip collision detection if particles are not close enough
// 	            if(abs(positionA.x - positionB.x) >= radiiSum)
// 	            	continue;

// 	            if(abs(positionA.y - positionB.y) >= radiiSum)
// 	            	continue;

// 	             //Avoid collision checks with the same particle
// 	            if (a == b)
// 		        	continue;

// 	            //Skip collision detection if particles are not close enough
// 	            if(positionA.distanceSquared(positionB) > radiiSum * radiiSum)
// 	            	continue;

//                 // Calculate the distance by which the particles overlap
//                 overlapDistance = radiiSum - positionA.distance(positionB);

//                 if(overlapDistance <= 0)
//                 	continue;

//     			velocityB = b -> getVelocity();

                
//                 // Calculate the direction vector of overlap
//                 overlap = positionA.getVector(positionB);

//                 overlap.normalize(overlapDistance);

//                 // Calculate the area of the particle
//                 areaB = b -> getArea();

//                 // Calculate the total area for overlap adjustment
//                 totalArea = areaA + areaB;

//                 // Calculate factors for adjusting particle positions
//                 factorA = areaA / totalArea;
//                 factorB = areaB / totalArea;

//                 // Adjust particle positions to resolve overlap
//                 positionA += {overlap.x * factorB, overlap.y * factorB};
//                 positionB -= {overlap.x * factorA, overlap.y * factorA};  

//                 // Calculate the collision normal direction
//                 collisionNormal = positionA.getVector(positionB);
//                 collisionNormal.normalize(1);

//                 // Calculate the relative velocity along the collision normal
//                 velocity = velocityB - velocityA;
//                 relativeVelocity = velocity.dot(collisionNormal);

//                 // Calculate the collision impulse for energy exchange
//                 impulse = -((1 + restitution) * relativeVelocity) / (1 / (areaA * density) + 1 / (areaB * density));

//                 // Update velocities based on collision impulse and energy loss
//                 velocityA.x -= (impulse / (areaA * density) * collisionNormal.x * (1 - energyLoss));
//                 velocityA.y -= (impulse / (areaA * density) * collisionNormal.y * (1 - energyLoss));

//                 velocityB.x += (impulse / (areaB * density) * collisionNormal.x * (1 - energyLoss));
//                 velocityB.y += (impulse / (areaB * density) * collisionNormal.y * (1 - energyLoss));

//                 b -> setPosition(positionB);
//                 b -> setVelocity(velocityB);
// 		    }
// 		    // for (auto& l : staticLines)
// 		    // {
		    	
// 		    // }

//     		a -> setPosition(positionA);
//     		a -> setVelocity(velocityA);
// 		}
//     }
// }

