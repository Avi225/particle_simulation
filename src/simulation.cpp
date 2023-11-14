#include "simulation.hpp"

// Particle class constructor
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
double staticLine::checkParticleCollision(particle target)
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

	running = false;
	isPlacingParticle = false;
	iterationSteps = 16;
}

// Update (tick) the simulation
void simulationContainer::update()
{
	//printf("%i\n", particles.size());
	if(running)
	{
		// Create pairs of particle indices to check for collisions
		std::vector<std::pair<int, int>> pairs;

		for(int i = 0; i < int(particles.size()); ++i)
			for(int j = i + 1; j < int(particles.size()); j++)
				if(i != j)
					pairs.push_back(std::make_pair(i, j));

		// Update particle positions and velocities
		for(int i = 0; i < int(particles.size()); i++)
		{
			// Accelerate particles using their assigned accelerations
			particles[i].velocity.x += particles[i].acceleration.x;
			particles[i].velocity.y += particles[i].acceleration.y;

			// Update particle position based on its velocity
			particles[i].position.x += particles[i].velocity.x;
			particles[i].position.y += particles[i].velocity.y;
		
			// Apply a small friction-like force to slow down particles over time
			particles[i].velocity.x += (particles[i].velocity.x > 0) ? -0.001 : 0.001;
			particles[i].velocity.y += (particles[i].velocity.y > 0) ? -0.001 : 0.001;
		}
		
		for(int ii = 0; ii < iterationSteps; ++ii)
		{
			// Collision resolution for particles
			for(int i = 0; i < int(pairs.size()); ++i)
			{
				if(particles[pairs[i].first].checkCollision(particles[pairs[i].second]))
				{
					// Calculate the amount of overlap between the two particles
					double difference = particles[pairs[i].first].radius + particles[pairs[i].second].radius - particles[pairs[i].first].position.distance(particles[pairs[i].second].position);
					
					// Calculate the vector that represents the overlap direction
					vector2d overlap = particles[pairs[i].first].position.getVector(particles[pairs[i].second].position);
					overlap.normalize(difference);
					
					// Adjust positions to resolve overlap
					particles[pairs[i].first].position.x += overlap.x / 2;
					particles[pairs[i].first].position.y += overlap.y / 2;
					particles[pairs[i].second].position.x -= overlap.x / 2;
					particles[pairs[i].second].position.y -= overlap.y / 2;

					// Calculate collision normal direction
					vector2d collisionNormal = particles[pairs[i].first].position.getVector(particles[pairs[i].second].position);
					double distance = sqrt(collisionNormal.x * collisionNormal.x + collisionNormal.y * collisionNormal.y);
					collisionNormal.x /= distance;
					collisionNormal.y /= distance;

					// Calculate relative velocity along the collision normal
					vector2d velocity = {particles[pairs[i].second].velocity.x - particles[pairs[i].first].velocity.x, particles[pairs[i].second].velocity.y - particles[pairs[i].first].velocity.y};
					double relativeVelocity = velocity.dot(collisionNormal);
					
					// Calculate collision impulse for energy exchange
					double impulse = (-(1 + restitution) * relativeVelocity) / (1/(particles[pairs[i].first].getArea()*density) + 1/(particles[pairs[i].second].getArea()*density));

					// Update velocities based on collision impulse, subtracting energy loss
					particles[pairs[i].first].velocity.x -= (impulse / (particles[pairs[i].first].getArea()*density) * collisionNormal.x * (1 - energyLoss));
					particles[pairs[i].first].velocity.y -= (impulse / (particles[pairs[i].first].getArea()*density) * collisionNormal.y * (1 - energyLoss));
					particles[pairs[i].second].velocity.x += (impulse / (particles[pairs[i].second].getArea()*density) * collisionNormal.x * (1 - energyLoss));
					particles[pairs[i].second].velocity.y += (impulse / (particles[pairs[i].second].getArea()*density) * collisionNormal.y * (1 - energyLoss));

				}
			}

			// Collision resolution with static lines
			for(int i = 0; i < int(particles.size()); ++i)
			{
				for(int iii = 0; iii < int(staticLines.size()); ++iii)
				{
					if(staticLines[iii].checkParticleCollision(particles[i]) < particles[i].radius)
					{
						// Get the normal vector of the static line
						vector2d normal = staticLines[iii].getNormal();
						normal.normalize(1);

						// Adjust positions to resolve overlap plus a small distance for stability
						particles[i].position.x += (particles[i].radius - staticLines[iii].checkParticleCollision(particles[i]) + 0.01) * normal.x;
						particles[i].position.y += (particles[i].radius - staticLines[iii].checkParticleCollision(particles[i]) + 0.01) * normal.y;
						
						// Calculate the dot product of particle velocity and normal vector
						double dot = 2 * (particles[i].velocity.x * normal.x + particles[i].velocity.y * normal.y);
						
						 // Reflect the particle's velocity based on the collision normal and the cooficient of restitution
						particles[i].velocity.x -= dot * normal.x * restitution * (1 - energyLoss);
						particles[i].velocity.y -= dot * normal.y * restitution * (1 - energyLoss);

					}
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
