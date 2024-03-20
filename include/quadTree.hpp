#pragma once

#include <vector>
#include <string>

#include "math.hpp"
#include "utility.hpp"
#include "aCamera.hpp"
#include "simulationElements.hpp"

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
	void render(aCamera* camera, vector2d mouse);
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