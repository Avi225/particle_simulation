#include "simulationElements.hpp"

// Particle class constructor
particle::particle()
: position({0, 0}), radius(1)
{
	velocity = {0, 0};
	acceleration = {0, 0.03}; // Apply a small downwards gravity force
}

// Particle class constructor with specified position and radius
particle::particle(vector2d nPosition, double nRadius)
: position(nPosition), radius(nRadius)
{
	velocity = {0, 0};
	acceleration = {0, 0.03}; // Apply a small downwards gravity force
}

// Particle move constructor
// particle::particle(particle&& other)
// : position(std::move(other.position)),
//   velocity(std::move(other.velocity)),
//   acceleration(std::move(other.acceleration)),
//   radius(other.radius),
//   mutex_(std::move(other.mutex_)) 
// {}

// // Particle move assignment operator
// particle& particle::operator=(particle&& other)
// {
//     if (this != &other)
//     {
//         position = std::move(other.position);
//         velocity = std::move(other.velocity);
//         acceleration = std::move(other.acceleration);
//         radius = other.radius;
//         mutex_ = std::move(other.mutex_);
//     }
//     return *this;
// }

// Render the particle
void particle::render(aCamera *camera)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderDisc(position, radius, color, false);
}

// Render the debug overlay for the particle
void particle::renderDebug(aCamera *camera)
{
	SDL_Color color = {255, 255, 0, 128};

	camera -> renderDisc(position, radius, color, false);

	vector2d nVelocity = 
	{
		position.x+velocity.x*10,
		position.y+velocity.y*10
	};

	camera -> renderLine(position, nVelocity, 0.1, {255, 0, 0, 255}, false);
}

// Calculate the area of the particle
double particle::getArea()
{
	return 3.14159265359 * radius * radius;
}

void particle::setPosition(vector2d nPosition)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	position = nPosition;
}

void particle::setVelocity(vector2d nVelocity)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	velocity = nVelocity;
}

void particle::setAcceleration(vector2d nAcceleration)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	acceleration = nAcceleration;
}

void particle::setRadius(double nRadius)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	radius = nRadius;
}

void particle::addPosition(vector2d nPosition)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	position += nPosition;
}

void particle::addVelocity(vector2d nVelocity)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	velocity += nVelocity;
}

void particle::addAcceleration(vector2d nAcceleration)
{	
	//std::lock_guard<std::shared_mutex> lock(mtx);
	acceleration += nAcceleration;
}

void particle::addRadius(double nRadius)
{
	//std::lock_guard<std::shared_mutex> lock(mtx);
	radius += nRadius;
}

vector2d particle::getPosition()
{
	return position;
}

vector2d particle::getVelocity()
{
	return velocity;
}

vector2d particle::getAcceleration()
{
	return acceleration;
}

double particle::getRadius()
{
	return radius;
}

// StaticPoint class constructor
staticPoint::staticPoint(vector2d nPosition)
: position(nPosition)
{}

// StaticLine class constructor
staticLine::staticLine(staticPoint* na, staticPoint* nb)
: a(na), b(nb)
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
	camera -> renderLine((a->position + b->position) / 2, (a->position + b->position) / 2 + (normal / 16), 0.1, {0, 0, 255, 255}, false);
}

// Get the normal vector of the static line
vector2d staticLine::getNormal()
{
	vector2d normal = {(b->position.y - a->position.y), -(b->position.x - a->position.x)};
	return normal;
}

// Check if a particle collides with this static line
double staticLine::checkParticleCollision(particle* target)
{
	// Calculate vector from the line to the particle's position
	vector2d v = {target -> getPosition() - a->position};

	// Calculate direction vector of the line
	vector2d direction = {b->position.getVector(a->position)};

	vector2d normal = getNormal();

	double length = a->position.distance(b->position);

	// Calculate projection of the particle's position onto the line segment
	double projection = (v.x * direction.x + v.y * direction.y) / length;

	// Check if the projection falls within the line segment
	if (projection >= 0 && projection <= length)
	{
		// Calculate distance between the particle and the line
		double distance = aAbs(v.x * normal.x + v.y * normal.y) / length;
		return distance;
	}

	// Return a distance greater than the sum of the particle's and line's radii if no collision
	return target -> getRadius() + 1;
}
