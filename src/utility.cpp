#include "utility.hpp"

grid::grid(vector2f position, int w, int h, float cellW, float cellH)
:width(w), height(h), cellWidth(cellW), cellHeight(cellH)
{}

void grid::render(aCamera *camera)
{
	SDL_Color color = {50, 50, 50, 255};
	vector2f a, b;
	for(int i = 0; i < width; i++)
	{
		a = {
			position.x - cellWidth * width / 2 + i * cellWidth + cellWidth/2,
			position.y - cellHeight * height / 2 + cellHeight/2
			};

		b = {
			position.x - cellWidth * width / 2 + i * cellWidth + cellWidth/2,
			position.y + cellHeight * height / 2 - cellHeight + cellHeight/2
			};
		
		camera -> renderLine(a, b, 0.1, color);
	}

	for(int i = 0; i < height; i++)
	{
		a = {
			position.x - cellWidth * width / 2 + cellWidth/2,
			position.y - cellHeight * height / 2 + i * cellHeight + cellHeight/2
			};

		b = {
			position.x + cellWidth * width / 2 - cellWidth + cellWidth/2,
			position.y - cellHeight * height / 2 + i * cellHeight + cellHeight/2
			};
		camera -> renderLine(a, b, 0.1, color);
	}
}
