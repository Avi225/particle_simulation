#include "simulation.hpp"

// Particle class constructor

particle::particle()
: position({0, 0}), radius(1)
{
	velocity = {0, 0};
	acceleration = {0, 0.01}; // Apply a small downards gravity force (TODO: move gravity application outside of the constructor)
	updated = false;
}


particle::particle(vector2d nPosition, double nRadius)
: position(nPosition), radius(nRadius)
{
	velocity = {0, 0};
	acceleration = {0, 0.01}; // Apply a small downards gravity force (TODO: move gravity application outside of the constructor)
	updated = false;
}

// StaticPoint class constructor
staticPoint::staticPoint(vector2d nPosition)
: position(nPosition)
{}

// StaticLine class constructor
staticLine::staticLine(staticPoint* na, staticPoint* nb)
:a(na), b(nb)
{}

// Render the static line
void staticLine::render(aCamera *camera)
{
	camera -> renderLine(a->position, b->position, 0.01, {255, 255, 255, 255}, false);
}

// Render the normal vector of the static line
void staticLine::renderNormal(aCamera *camera)
{
	vector2d normal = getNormal();
	normal.normalize(1);
	camera -> renderLine((a->position + b->position) / 2, (a->position + b->position) / 2 + normal, 0.1, {0, 0, 255, 255}, false);
}

// Get the normal vector of the static line
vector2d staticLine::getNormal()
{
	vector2d normal = {(b->position.y - a->position.y), -(b->position.x - a->position.x)};
	return(normal);
}

// Check if a particle collides with this static line
double staticLine::checkParticleCollision(const particle& target)
{
	vector2d v = {target.position.x - a->position.x, target.position.y - a->position.y};
	vector2d dir = {b->position.getVector(a->position)};
	vector2d normal = getNormal();
	double len = a->position.distance(b->position);
	double pro = (v.x * dir.x + v.y * dir.y) / len;
	if(pro >= 0 && pro <= len)
	{
		double dis = aAbs(v.x * normal.x + v.y * normal.y) / len;
		return dis;
	}
	return target.radius+1;
}

// Render the particle
void particle::render(aCamera *camera)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderDisc(position, radius, color, false);
}

// Render the velocity vector of the particle
void particle::renderVector(aCamera *camera)
{
	vector2d nVelocity = 
	{
		position.x+velocity.x*10,
		position.y+velocity.y*10
	};

	camera -> renderLine(position, nVelocity, 0.1, {255, 0, 0, 255}, false);
}

// Check if two particles collide
bool particle::checkCollision(particle b)
{
	if(position.distance(b.position) < (radius + b.radius))
		return true;	
	else
		return false;
}

// Calculate the area of the particle
double particle::getArea()
{
	return(3.14159265359 * radius * radius);
}

void particle::lock()
{
	mutex_.mutex_ptr -> lock();
}

void particle::unlock()
{
	mutex_.mutex_ptr -> unlock();
}

quadTreeBox::quadTreeBox()
{}

quadTreeBox::quadTreeBox(vector2d nCenter, double nHalfDimension)
:center(nCenter), halfDimension(nHalfDimension)
{}

quadTree::quadTree(quadTreeBox nBoundary, int nCapacity)
:boundary(nBoundary), capacity(nCapacity)
{
	nw = nullptr;
	ne = nullptr;
	sw = nullptr;
	se = nullptr;
}

void quadTree::split()
{
	if(int(particles.size()) <= capacity)
		return;
	if(boundary.halfDimension*2 <= 0.5)
		return;

	quadTreeBox nBoundary;
	nBoundary.halfDimension = boundary.halfDimension / 2;

	nBoundary.center.x = boundary.center.x - (boundary.halfDimension / 2);
	nBoundary.center.y = boundary.center.y - (boundary.halfDimension / 2);
	nw = new quadTree(nBoundary, capacity);

	nBoundary.center.x = boundary.center.x + (boundary.halfDimension / 2);
	nBoundary.center.y = boundary.center.y - (boundary.halfDimension / 2);
	ne = new quadTree(nBoundary, capacity);

	nBoundary.center.x = boundary.center.x - (boundary.halfDimension / 2);
	nBoundary.center.y = boundary.center.y + (boundary.halfDimension / 2);
	sw = new quadTree(nBoundary, capacity);

	nBoundary.center.x = boundary.center.x + (boundary.halfDimension / 2);
	nBoundary.center.y = boundary.center.y + (boundary.halfDimension / 2);
	se = new quadTree(nBoundary, capacity);

	for (auto& p : particles)
	{
		if(p -> position.x >= (boundary.center.x - p -> radius))
		{
			if(p -> position.y >= (boundary.center.y - p -> radius))
				se -> insertParticle(p);

			if(p -> position.y <= (boundary.center.y + p -> radius))
				ne -> insertParticle(p);
		}
		if(p -> position.x <= (boundary.center.x + p -> radius))
		{
			if(p -> position.y >= (boundary.center.y - p -> radius))
				sw -> insertParticle(p);

			if(p -> position.y <= (boundary.center.y + p -> radius))
				nw -> insertParticle(p);
		}
	}

	particles.clear();
	particles.shrink_to_fit();
	
	nw -> split();
	ne -> split();
	sw -> split();
	se -> split();
}

void quadTree::render(aCamera* camera)
{
	SDL_Color color = {0, 128, 255, 86};

	vector2d a = {boundary.center.x - boundary.halfDimension,
				  	boundary.center.y - boundary.halfDimension};

	vector2d b = {boundary.center.x - boundary.halfDimension,
				  	boundary.center.y + boundary.halfDimension};
	camera -> renderLine(a, b, 0.1, color, false);

	a = {boundary.center.x + boundary.halfDimension,
		boundary.center.y + boundary.halfDimension};

	b = {boundary.center.x + boundary.halfDimension,
		boundary.center.y - boundary.halfDimension};
	camera -> renderLine(a, b, 0.1, color, false);


	a = {boundary.center.x - boundary.halfDimension,
		boundary.center.y - boundary.halfDimension};

	b = {boundary.center.x + boundary.halfDimension,
		boundary.center.y - boundary.halfDimension};
	camera -> renderLine(a, b, 0.1, color, false);


	a = {boundary.center.x - boundary.halfDimension,
		boundary.center.y + boundary.halfDimension};

	b = {boundary.center.x + boundary.halfDimension,
		boundary.center.y + boundary.halfDimension};

	camera -> renderLine(a, b, 0.1, color, false);


	if(nw != nullptr)
	{
		nw -> render(camera);
		ne -> render(camera);
		sw -> render(camera);
		se -> render(camera);
	}
	
}

void quadTree::insertParticle(particle* p)
{
	particles.push_back(p);
}

void quadTree::clear()
{
	if (nw)
	{
        nw->clear();
        delete nw;
        nw = nullptr;
    }

    if (ne)
    {
        ne->clear();
        delete ne;
        ne = nullptr;
    }

    if (sw)
    {
        sw->clear();
        delete sw;
        sw = nullptr;
    }

    if (se)
    {
        se->clear();
        delete se;
        se = nullptr;
    }
}

void quadTree::getLeaves(std::vector<quadTree*>& quads)
{
	if(nw == nullptr)
	{
		quads.push_back(this);
	}
	else
	{
		nw -> getLeaves(quads);
		ne -> getLeaves(quads);
		sw -> getLeaves(quads);
		se -> getLeaves(quads);
	}
}

// Constructor for the simulation container
simulationContainer::simulationContainer()
{
	density = 1; // Universal density of each particle (TODO: custom density for each)
	restitution = 0.7; // 1: Perfect bounce, all kinetic energy is conserved, 0: No bounce, all kinetic energy is lost.
	energyLoss = 0;
	friction = 0.001;
    overlapGap = 0.01;

	running = false;

	quadrantCapacity = 128;

	nodeQuadTree = new quadTree({vector2d(0, 0), 262144}, quadrantCapacity);

	isPlacingParticle = false;
	iterationSteps = 16;
}


void simulationContainer::update()
{
    particleCount = int(particles.size());

    nodeQuadTree -> clear();
    nodeQuadTree = new quadTree({vector2d(0, 0), 262144}, quadrantCapacity);

    for (auto& p : particles)
    {
    	nodeQuadTree -> insertParticle(&p);
    	p.updated = false;
    }

    nodeQuadTree -> split();

    std::vector<quadTree*> quads;
    nodeQuadTree -> getLeaves(quads);

    std::vector<std::thread> workers;

    for (auto& q : quads)
    {
    	workers.push_back(std::thread(&simulationContainer::worker, this, q));
    }
    for (auto& w : workers)
    {
    	w.join();
    }
}

// Render the simulation
void simulationContainer::render(aCamera *camera)
{
	// Render placing particle preview line if needed
	if(isPlacingParticle)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		vector2d mouse = {double(x), double(y)};
		mouse = camera -> screenToWorld(mouse);
		camera -> renderLine(placeParticlePosition, mouse, 0.1, {255, 0, 0, 255}, false);
	}

 	// Render particles and static lines with respectively velocity and normal vectors 
	for(int i = 0; i < int(particles.size()); i++)
	{
		particles[i].render(camera);
		//particles[i].renderVector(camera);
	}

	for(int i = 0; i < int(staticLines.size()); i++)
	{
		staticLines[i].render(camera);
		staticLines[i].renderNormal(camera);
	}
}

void simulationContainer::renderQuadTree(aCamera *camera)
{
	if(nodeQuadTree != nullptr)
		nodeQuadTree -> render(camera);
}


// Start placing and place a particle at a specified position with velocity vector pointing towards the mouse release location
void simulationContainer::placeParticle(vector2d position, double radius, bool state)
{
	if(!state)
	{
		placeParticlePosition = position;
		isPlacingParticle = true;
	}
	else
	{
		particles.push_back(particle(placeParticlePosition, radius));
		particles[particles.size()-1].velocity = placeParticlePosition.getVector(position);
		particles[particles.size()-1].velocity.x /= -10;
		particles[particles.size()-1].velocity.y /= -10;
		isPlacingParticle = false;

	}
}

void simulationContainer::addStaticPoint(vector2d position)
{
	staticPoints.push_back(staticPoint(position));
}

void simulationContainer::addStaticLine(int a, int b)
{
	staticLines.push_back(staticLine(&staticPoints[a], &staticPoints[b]));
}

// Toggle the simulation running state
void simulationContainer::switchRunning()
{
	running = running ? false : true;
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
// Get a pointer to a particle by its ID in the `particles` vector
particle* simulationContainer::getParticle(int id)
{
	return &particles[id];
}

int* simulationContainer::getParticleCount()
{
	return &particleCount;
}

void simulationContainer::worker(quadTree* q)
{
	
	for (auto& p : q -> particles)
    {
    	if(p -> updated)
    		continue;
    	p -> lock();
        // Calculate friction forces
        double frictionX = (p -> velocity.x > 0) ? -friction : friction;
        double frictionY = (p -> velocity.y > 0) ? -friction : friction;

        // Update p velocities with accelerations
        p -> velocity.x += p -> acceleration.x;
        p -> velocity.y += p -> acceleration.y;

        // Update particle positions based on velocities
        p -> position.x += p -> velocity.x;
        p -> position.y += p -> velocity.y;

        // Apply friction-like force
        p -> velocity.x += frictionX;
        p -> velocity.y += frictionY;
    }
    for (int ii = 0; ii < iterationSteps; ++ii)
    {
        // Collision resolution for particles
        for (auto& a : q -> particles)
        {
            for (auto& b : q -> particles)
            {
                if (a != b)
                {
                	//printf("%i, %i \n", a, b);
                    // Calculate particle radii sum and overlap distance
                    double radiiSum = a -> radius + b -> radius;
                    if((radiiSum*radiiSum) >= a -> position.distanceSquared(b -> position))
                    {
                    	//printf("checked");
                    	double overlapDistance = radiiSum - a -> position.distance(b -> position);
                        // Calculate overlap direction vector
                        vector2d overlap = a -> position.getVector(b -> position);
                        overlap.normalize(overlapDistance);

                        // Adjust particle positions to resolve overlap

                        double t = a -> getArea() + b -> getArea();
                        double fa = a -> getArea() / t;
                        double fb = b -> getArea() / t;

                		//printf("%f, %f \n", fa, fb);


                        a -> position.x += overlap.x * fb;
                        a -> position.y += overlap.y * fb;
                        b -> position.x -= overlap.x * fa;
                        b -> position.y -= overlap.y * fa;

                        // Calculate collision normal direction
                        vector2d collisionNormal = a -> position.getVector(b -> position);
                        collisionNormal.normalize(1);

                        // Calculate relative velocity along collision normal
                        vector2d velocity = {b -> velocity.x - a -> velocity.x,
                                             b -> velocity.y - a -> velocity.y};
                        double relativeVelocity = velocity.dot(collisionNormal);

                        // Calculate collision impulse for energy exchange
                        double impulse = -((1 + restitution) * relativeVelocity) / (1 / (a -> getArea() * density) + 1 / (b -> getArea() * density));

                        // Update velocities based on collision impulse
                        a -> velocity.x -= (impulse / (a -> getArea() * density) * collisionNormal.x * (1 - energyLoss));
                        a -> velocity.y -= (impulse / (a -> getArea() * density) * collisionNormal.y * (1 - energyLoss));
                        b -> velocity.x += (impulse / (b -> getArea() * density) * collisionNormal.x * (1 - energyLoss));
                        b -> velocity.y += (impulse / (b -> getArea() * density) * collisionNormal.y * (1 - energyLoss));
                    }
                }
            }       
        }
        //printf("\n");

        // Collision resolution with static lines
        for (auto& p : q -> particles)
        {
            for (auto& staticLine : staticLines)
            {
                double overlap = staticLine.checkParticleCollision(*p);
                if (overlap < p -> radius)
                {
                    // Get normal vector of static line
                    vector2d normal = staticLine.getNormal();
                    normal.normalize(1);

                    // Adjust particle positions to resolve overlap
                    p -> position.x += (p -> radius - overlap + overlapGap) * normal.x;
                    p -> position.y += (p -> radius - overlap + overlapGap) * normal.y;

                    // Calculate dot product of particle velocity and normal vector
                    double dot = 2 * (p -> velocity.x * normal.x + p -> velocity.y * normal.y);

                    // Reflect particle velocity based on collision normal and restitution factor
                    p -> velocity.x -= dot * normal.x * (1 - energyLoss) * restitution;
                    p -> velocity.y -= dot * normal.y * (1 - energyLoss) * restitution;
                }
            }
            p -> updated = true;
            p -> unlock();
        }
    }
}

