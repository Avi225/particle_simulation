#include "interface.hpp"

void tabElement::render(aCamera* camera, vector2d position)
{}

tabText::tabText(std::string nValue, int nSize, std::string info)
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

tabSliderD::tabSliderD(double* nValue, double nMinValue, double nMaxValue, std::string info)
: value(nValue), minValue(nMinValue), maxValue(nMaxValue)
{
	height = 22;
	adjustment = {0, 11};
}

void tabSliderD::render(aCamera* camera, vector2d position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderLine(position, vector2d(position.x+150, position.y), 2, color, true);
	vector2d pointPosition = position;
	pointPosition.x += mapRange(*value, minValue, maxValue, 0, 150);
	camera -> renderDisc(pointPosition, 5, color, true);
}

tabDisplayI::tabDisplayI(int* nValue, int nSize, std::string nText, std::string info)
:value(nValue), size(nSize), text(nText)
{
	height = 22*size;
	adjustment = {0, 0};
}

void tabDisplayI::render(aCamera* camera, vector2d position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderText(position, size, text + std::to_string(*value), color, 1);
}

tabDisplayD::tabDisplayD(double* nValue, int nSize, std::string nText, std::string info)
:value(nValue), size(nSize), text(nText)
{
	height = 22*size;
	adjustment = {0, 0};
}

void tabDisplayD::render(aCamera* camera, vector2d position)
{
	SDL_Color color = {255, 255, 255, 255};
	camera -> renderText(position, size, text + std::to_string(*value), color, 1);
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
		160+margin*2,
		totalHeight+margin*2
	};
	SDL_Color color = {0, 0, 0, 225};
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
}

void menu::insertTab(tab* nTab)
{
	tabs.push_back(nTab);
	tabStates.push_back({false, false, true});
}

void menu::updateTabs()
{
	int x, y;
	SDL_Rect rect;

	bool mouseOver = false;
	bool LMBdown = false;

	if(SDL_GetMouseState(&x,&y) & SDL_BUTTON_LMASK)
		LMBdown = true;


	for(int i = 0; i < int(tabs.size()); ++i)
	{
		rect =
			{
				int(position.x)+i*104,
				int(position.y),
				96,
				33
			};
		mouseOver = x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h;

		if(tabStates[i][1] && !LMBdown && mouseOver)
		{
			for(int ii = 0; ii < int(tabs.size()); ++ii)
			{	
				if(ii==i)
					tabStates[ii][2] = !tabStates[ii][2];
				else
					tabStates[ii][2] = false;	
			}
		}

        tabStates[i][0] = (mouseOver) ? true : false;
        tabStates[i][1] = (tabStates[i][0] && LMBdown) ? true : false;
        printf("%i, %i, %i || ", int(tabStates[i][0]), int(tabStates[i][1]), int(tabStates[i][2]));
   		
 
	}
	//printf("\n\n");
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

		color = {0, 0, 0, 0};
		color.a = 128+48*(tabStates[i][0]+tabStates[i][1]);
		if(tabStates[i][2])
			color = {0, 0, 0, 223};

		camera -> renderRect(rect, color, true);

		color = {255, 255, 255, 255};
		if(tabStates[i][2])
			color = {255, 255, 255, 255};

		camera -> renderText(vector2d(position.x+i*104+48-(tabs[i]->getName().length()*10/2), position.y+4), 1, tabs[i]->getName(), color, true);
		if(tabStates[i][2])
			tabs[i] -> render(camera, position);
	}
}

