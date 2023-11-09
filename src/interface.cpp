#include "interface.hpp"

tabText::tabText(std::string nValue, int nSize)
:value(nValue), size(nSize)
{
	height = 22*size;
	adjustment = {0, 0};
}

void tabText::render(aCamera* camera, vector2d position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderText(position, size, value, color, 1);
}

tabBreak::tabBreak()
{
	height = 22;
	adjustment = {0, 0};
}

void tabBreak::render(aCamera* camera, vector2d position)
{}

tabSliderF::tabSliderF(double* nValue, double nMinValue, double nMaxValue)
: value(nValue), minValue(nMinValue), maxValue(nMaxValue)
{
	height = 22;
	adjustment = {0, 11};
}

void tabSliderF::render(aCamera* camera, vector2d position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderLine(position, vector2d(position.x+150, position.y), 2, color, true);
	vector2d pointPosition = position;
	pointPosition.x += mapRange(*value, minValue, maxValue, 0, 150);
	camera -> renderDisc(pointPosition, 5, color, true);
}

tab::tab(vector2d nPosition, std::string nName)
: position(nPosition), name(nName)
{
	margin = 20;
}

void tab::insertElement(tabElement* element)
{
	elements.push_back(element);
}

void tab::render(aCamera* camera, vector2d globalPosition)
{
	int totalHeight = 0;
	for(int i = 0; i < int(elements.size()); ++i)
	{
		totalHeight += elements[i] -> height;
	}

	SDL_Rect rect =
	{
		int(position.x+globalPosition.x),
		int(position.y+globalPosition.y),
		150+margin*2,
		totalHeight+margin*2
	};
	SDL_Color color = {0, 0, 0, 64};
	camera -> renderRect(rect, color, true);
	
	vector2d elementPosition = position + globalPosition + margin;

	for(int i = 0; i < int(elements.size()); ++i)
	{
		elements[i] -> render(camera, elementPosition+elements[i] -> adjustment);
		elementPosition.y += elements[i] -> height;
	}
}

std::string tab::getName()
{
	return name;
}

menu::menu(vector2d nPosition)
: position(nPosition)
{
	margin = 20;
	activeTab = -1;
}

void menu::insertTab(tab* nTab)
{
	tabs.push_back(nTab);
}

void menu::render(aCamera* camera)
{
	SDL_Color color = {0, 0, 0, 64};
	for(int i = 0; i < int(tabs.size()); ++i)
	{
		SDL_Rect rect =
		{
			int(position.x)+i*104,
			int(position.y),
			96,
			33
		};
		color = {0, 0, 0, 64};
		camera -> renderRect(rect, color, true);
		color = {255, 255, 255, 255};
		camera -> renderText(vector2d(position.x+i*104+48-(tabs[i]->getName().length()*10/2), position.y+4), 1, tabs[i]->getName(), color, true);
	}
}

