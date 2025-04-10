#include "simulationElements.hpp"

// Particle class constructor
particle::particle()
: position({0, 0}), radius(1)
{
	velocity = {0, 0};
	acceleration = {0, 0.00};
}

// Particle class constructor with specified position and radius
particle::particle(vector2d nPosition, double nRadius)
: position(nPosition), radius(nRadius)
{
	velocity = {0, 0};
	acceleration = {0, 0.00};
}

// Render the particle
void particle::render(aCamera *camera)
{
	SDL_Color color = {255, 255, 255, 255};
	Uint8 factor = Uint8(mapRange(clamp(velocity.length(), 0, 50), 0, 50, 0, 255));
	color.g -= factor;
	color.b -= factor;
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
