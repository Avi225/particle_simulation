#include "interface.hpp"

menuText::menuText(std::string nValue, int nSize)
:value(nValue), size(nSize)
{
	height = 48*size;
}

void menuText::render(aCamera* camera, vector2f position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderText(position, size, value, color, 1);
}

menuBreak::menuBreak()
{
	height = 48;
}

void menuBreak::render(aCamera* camera, vector2f position)
{}

menuSliderF::menuSliderF(float* nValue, float nMinValue, float nMaxValue)
: value(nValue), minValue(nMinValue), maxValue(nMaxValue)
{
	height = 64;
}

void menuSliderF::render(aCamera* camera, vector2f position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderLine(position, vector2f(position.x+150, position.y), 2, color, true);
	vector2f pointPosition = position;
	pointPosition.x += mapRange(*value, minValue, maxValue, 0, 150);
	camera -> renderDisc(pointPosition, 4, color, true);
}

menu::menu(vector2f nPosition)
: position(nPosition)
{}

void menu::insertElement(menuElement* element)
{
	elements.push_back(element);
}

void menu::render(aCamera* camera)
{
	int totalHeight = 0;
	for(int i = 0; i < int(elements.size()); ++i)
		totalHeight += elements[i] -> height;
	SDL_Rect rect =
	{
		int(position.x),
		int(position.y),
		150,
		totalHeight
	};
	SDL_Color color = {0, 0, 0, 64};
	camera -> renderRect(rect, color, true);
	
	vector2f elementPosition = position;
	for(int i = 0; i < int(elements.size()); ++i)
	{
		elementPosition.y += elements[i] -> height/2;
		elements[i] -> render(camera, elementPosition);
	}
}

