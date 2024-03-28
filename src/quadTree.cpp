#include "quadTree.hpp"

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

// Split the quadtree into four quadrants
void quadTree::split()
{
	// Check if splitting is necessary
	if((int(particles.size()) <= capacity) || (boundary.halfDimension/2 <= 2))
		return;

	// Define the boundaries of the quadrants
	quadTreeBox nBoundary;
	nBoundary.halfDimension = boundary.halfDimension / 2;

	// Northwest quadrant
	nBoundary.center.x = boundary.center.x - (boundary.halfDimension / 2);
	nBoundary.center.y = boundary.center.y - (boundary.halfDimension / 2);
	nw = new quadTree(nBoundary, capacity);

	// Southwest quadrant
	nBoundary.center.y = boundary.center.y + (boundary.halfDimension / 2);
	sw = new quadTree(nBoundary, capacity);

	// Southeast quadrant
	nBoundary.center.x = boundary.center.x + (boundary.halfDimension / 2);
	se = new quadTree(nBoundary, capacity);

	// Northeast quadrant
	nBoundary.center.x = boundary.center.x + (boundary.halfDimension / 2);
	nBoundary.center.y = boundary.center.y - (boundary.halfDimension / 2);
	ne = new quadTree(nBoundary, capacity);

	

	// Distribute particles among the quadrants
	for (particle* p : particles)
	{
		vector2d position = p -> getPosition();
		double radius = p -> getRadius();
		
		if(position.x >= (boundary.center.x - radius * 2))
		{
			if(position.y >= (boundary.center.y - radius * 2))
				se -> particles.push_back(p);

			if(position.y <= (boundary.center.y + radius * 2))
				ne -> particles.push_back(p);
		}
		if(position.x <= (boundary.center.x + radius * 2))
		{
			if(position.y >= (boundary.center.y - radius * 2))
				sw -> particles.push_back(p);

			if(position.y <= (boundary.center.y + radius * 2))
				nw -> particles.push_back(p);
		}
	}

	// Clear particles from this quadrant
	particles.clear();
	particles.shrink_to_fit();

	// Split the child quadrants recursively
	nw -> split();
	ne -> split();
	sw -> split();
	se -> split();
}

// Render the quadtree
void quadTree::render(aCamera* camera, vector2d mouse)
{
	if(nw != nullptr)
	{
		nw -> render(camera, mouse);
		ne -> render(camera, mouse);
		sw -> render(camera, mouse);
		se -> render(camera, mouse);
		return;
	}

	// Draw the boundary of the quadrant
	vector2d pos = {boundary.center.x - boundary.halfDimension, boundary.center.y - boundary.halfDimension};
	vector2d siz = {boundary.halfDimension * 2, boundary.halfDimension * 2};
	SDL_Color color = {0, 128, 255, 16};

	mouse = camera -> screenToWorld(mouse);
	if(mouse.x < pos.x+siz.x && mouse.x > pos.x && mouse.y < pos.y+siz.y && mouse.y > pos.y && nw == nullptr)
		color = {255, 0, 0, 64};

	camera -> renderRect(pos, siz, color, false);

	// Draw the lines of the quadrant
	color.a += 64;
	vector2d a = {boundary.center.x - boundary.halfDimension + 0.2, boundary.center.y - boundary.halfDimension + 0.2};
	vector2d b = {boundary.center.x - boundary.halfDimension + 0.2, boundary.center.y + boundary.halfDimension - 0.2};
	camera -> renderLine(a, b, 0.4, color, false);

	a = {boundary.center.x + boundary.halfDimension - 0.2, boundary.center.y + boundary.halfDimension - 0.2};
	b = {boundary.center.x + boundary.halfDimension - 0.2, boundary.center.y - boundary.halfDimension + 0.2};
	camera -> renderLine(a, b, 0.4, color, false);

	a = {boundary.center.x - boundary.halfDimension + 0.2, boundary.center.y - boundary.halfDimension + 0.2};
	b = {boundary.center.x + boundary.halfDimension - 0.2, boundary.center.y - boundary.halfDimension + 0.2};
	camera -> renderLine(a, b, 0.4, color, false);

	a = {boundary.center.x + boundary.halfDimension - 0.2, boundary.center.y + boundary.halfDimension - 0.2};
	b = {boundary.center.x - boundary.halfDimension + 0.2, boundary.center.y + boundary.halfDimension - 0.2};
	camera -> renderLine(a, b, 0.4, color, false);
}

// Insert a particle into the quadtree
void quadTree::insertParticle(particle* p)
{
	particles.push_back(p);
}

// Clear the whole quadtree and deallocate memory
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

// Retrieve all leaves of the quadtree
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