#pragma once

#include "math.hpp"
#include "aCamera.hpp"


class grid
{
public:
	grid(vector2f position, int w, int h, float cellW, float cellH);
	void render(aCamera *camera);

private:
	vector2f position;
	int width;
	int height;
	float cellWidth;
	float cellHeight;
};