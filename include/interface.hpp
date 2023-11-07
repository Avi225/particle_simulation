#pragma once

#include "aCamera.hpp"
#include "math.hpp"

#include <vector>
#include <string>

struct menuElement
{
	virtual void render(aCamera* camera, vector2f position);
	int height;
};

struct menuText : public menuElement
{
	menuText(std::string nValue, int nSize);
	void render(aCamera* camera, vector2f position);

	std::string value;
	int size;
};

struct menuBreak : public menuElement
{
	menuBreak();
	void render(aCamera* camera, vector2f position);
};

struct menuSliderF : public menuElement
{
	menuSliderF(float* nValue, float nMinValue, float nMaxValue);
	void render(aCamera* camera, vector2f position);

	float* value;
	float minValue;
	float maxValue;
};

class menu
{
public:
	menu(vector2f nPosition);
	void insertElement(menuElement* element);
	void render(aCamera* camera);

private:
	std::vector<menuElement*> elements;
	vector2f position;
};