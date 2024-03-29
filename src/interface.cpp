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

void tabSliderD::update()
{
	int x, y;

	bool LMBdown = false;

	if(SDL_GetMouseState(&x,&y) & SDL_BUTTON_LMASK)
		LMBdown = true;

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
 
state::state()
:hovered(false), pressed(false), selected(false), button(false)
{
}

state::state(bool nHovered, bool nPressed, bool nSelected, bool nButton)
:hovered(nHovered), pressed(nPressed), selected(nSelected), button(nButton)
{}

menu::menu(vector2d nPosition)
: position(nPosition)
{
	margin = 20;
}

void menu::insertTab(tab* nTab)
{
	tabs[nTab -> name] = nTab;
}

void menu::updateTabs()
{
	int x, y;
	SDL_Rect rect;

	bool LMBdown = false;
	int tabPosition = 0;


	if(SDL_GetMouseState(&x,&y) & SDL_BUTTON_LMASK)
		LMBdown = true;


	for(auto const& [name, tab] : tabs)
	{
		rect =
			{
				int(position.x)+tabPosition*104,
				int(position.y),
				96,
				33
			};

		bool mouseOver = (x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h);

		
		tab -> tabState.selected = (tab -> tabState.pressed && mouseOver && !LMBdown) ? !tab -> tabState.selected : tab -> tabState.selected;
		tab -> tabState.pressed = (LMBdown && tab -> tabState.hovered) ? true : false;
		tab -> tabState.hovered = (mouseOver && !LMBdown) || tab -> tabState.pressed;

		if(tab -> tabState.selected)
		{
			for(auto const& [n, t] : tabs)
				t -> tabState.selected = false;
			tab -> tabState.selected = true;
		}

		//printf(" %d %d %d ||", tab -> tabState.hovered, tab -> tabState.pressed, tab -> tabState.selected);
		
		tabPosition++;
	}
	//printf("\n");
}

void menu::render(aCamera* camera)
{
	SDL_Color color = {0, 0, 0, 128};
	int tabPosition = 0;
	for(auto const& [name, tab] : tabs)
	{
		SDL_Rect rect =
		{
			int(position.x)+tabPosition*104,
			int(position.y),
			96,
			33
		};

		camera -> renderRect(rect, color, true);
		SDL_Color color = {255, 255, 255, 255};
		color.a -= tab -> tabState.hovered * 64;
		color.a -= tab -> tabState.pressed * 64;
		color.a -= tab -> tabState.selected * 64;

		camera -> renderText(vector2d(position.x+tabPosition*104+48-(tab -> name.length()*10/2), position.y+4), 1, tab -> name, color, true);

		if(tab -> tabState.selected)
			tab -> render(camera, position);

		tabPosition++;
	}
}

