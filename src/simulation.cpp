#include "simulation.hpp"

// Particle class constructor

particle::particle()
: position({0, 0}), radius(1)
{
	velocity = {0, 0};
	acceleration = {0, 0.01}; // Apply a small downards gravity force (TODO: move gravity application outside of the constructor)
}


particle::particle(vector2d nPosition, double nRadius)
: position(nPosition), radius(nRadius)
{
	velocity = {0, 0};
	acceleration = {0, 0.01}; // Apply a small downards gravity force (TODO: move gravity application outside of the constructor)
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

// Constructor for the simulation container
simulationContainer::simulationContainer()
{
	density = 1; // Universal density of each particle (TODO: custom density for each)
	restitution = 0.7; // 1: Perfect bounce, all kinetic energy is conserved, 0: No bounce, all kinetic energy is lost.
	energyLoss = 0;
	friction = 0.001;
    overlapGap = 0.01;

	running = false;
	isPlacingParticle = false;
	iterationSteps = 16;
}


void simulationContainer::update()
{
    if (!running)
        return;

    for (auto& particle : particles)
    {
        // Calculate friction forces
        double frictionX = (particle.velocity.x > 0) ? -friction : friction;
        double frictionY = (particle.velocity.y > 0) ? -friction : friction;

        // Update particle velocities with accelerations
        particle.velocity.x += particle.acceleration.x;
        particle.velocity.y += particle.acceleration.y;

        // Update particle positions based on velocities
        particle.position.x += particle.velocity.x;
        particle.position.y += particle.velocity.y;

        // Apply friction-like force
        particle.velocity.x += frictionX;
        particle.velocity.y += frictionY;
    }

    for (int ii = 0; ii < iterationSteps; ++ii)
    {
        // Collision resolution for particles
        for (size_t a = 0; a < particles.size(); ++a)
        {
            for (size_t b = a + 1; b < particles.size(); ++b)
            {
                if (a != b)
                {
                    // Calculate particle radii sum and overlap distance
                    double radiiSum = particles[a].radius + particles[b].radius;
                    double overlapDistance = radiiSum - particles[a].position.distance(particles[b].position);

                    if (overlapDistance >= 0)
                    {
                        // Calculate overlap direction vector
                        vector2d overlap = particles[a].position.getVector(particles[b].position);
                        overlap.normalize(overlapDistance);

                        // Adjust particle positions to resolve overlap
                        particles[a].position.x += overlap.x / 2;
                        particles[a].position.y += overlap.y / 2;
                        particles[b].position.x -= overlap.x / 2;
                        particles[b].position.y -= overlap.y / 2;

                        // Calculate collision normal direction
                        vector2d collisionNormal = particles[a].position.getVector(particles[b].position);
                        collisionNormal.normalize(1);

                        // Calculate relative velocity along collision normal
                        vector2d velocity = {particles[b].velocity.x - particles[a].velocity.x,
                                             particles[b].velocity.y - particles[a].velocity.y};
                        double relativeVelocity = velocity.dot(collisionNormal);

                        // Calculate collision impulse for energy exchange
                        double impulse = -((1 + restitution) * relativeVelocity) / (1 / (particles[a].getArea() * density) + 1 / (particles[b].getArea() * density));

                        // Update velocities based on collision impulse
                        particles[a].velocity.x -= (impulse / (particles[a].getArea() * density) * collisionNormal.x * (1 - energyLoss));
                        particles[a].velocity.y -= (impulse / (particles[a].getArea() * density) * collisionNormal.y * (1 - energyLoss));
                        particles[b].velocity.x += (impulse / (particles[b].getArea() * density) * collisionNormal.x * (1 - energyLoss));
                        particles[b].velocity.y += (impulse / (particles[b].getArea() * density) * collisionNormal.y * (1 - energyLoss));
                    }
                }
            }
        }

        // Collision resolution with static lines
        for (auto& particle : particles)
        {
            for (auto& staticLine : staticLines)
            {
                double overlap = staticLine.checkParticleCollision(particle);
                if (overlap < particle.radius)
                {
                    // Get normal vector of static line
                    vector2d normal = staticLine.getNormal();
                    normal.normalize(1);

                    // Adjust particle positions to resolve overlap
                    particle.position.x += (particle.radius - overlap + overlapGap) * normal.x;
                    particle.position.y += (particle.radius - overlap + overlapGap) * normal.y;

                    // Calculate dot product of particle velocity and normal vector
                    double dot = 2 * (particle.velocity.x * normal.x + particle.velocity.y * normal.y);

                    // Reflect particle velocity based on collision normal and restitution factor
                    particle.velocity.x -= dot * normal.x * (1 - energyLoss) * restitution;
                    particle.velocity.y -= dot * normal.y * (1 - energyLoss) * restitution;
                }
            }
        }
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
		particles[i].renderVector(camera);
	}

	for(int i = 0; i < int(staticLines.size()); i++)
	{
		staticLines[i].render(camera);
		staticLines[i].renderNormal(camera);
	}

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
